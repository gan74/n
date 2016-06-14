/*******************************
Copyright (C) 2013-2015 gregoire ANGERAND

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
**********************************/
#include "Parser.h"
#include "ASTExpressions.h"
#include "ASTInstructions.h"

namespace n {
namespace script {

const char *SynthaxErrorException::what() const noexcept {
	buffer = "{ ";
	for(Token::Type t : expected) {
		buffer << Token::getName(t) << " ";
	}
	buffer = "Expected " + buffer + "} got " + Token::getName(token.type) + " (\"" + token.string + "\")";
	return buffer.data();
}

const char *SynthaxErrorException::what(const core::String &code) const noexcept {
	buffer = "{ ";
	for(Token::Type t : expected) {
		buffer << Token::getName(t) << " ";
	}
	buffer = "Expected " + buffer + "} got " + Token::getName(token.type) + ":";
	if(token.type == Token::Identifier) {
		buffer += " \"" + token.string + "\"";
	}
	buffer += "\n" + token.position.toString(code);

	return buffer.data();
}




void expected(core::Array<Token>::const_iterator it, const core::Array<Token::Type> &types) {
	throw SynthaxErrorException(types, *(--it));
}

void expect(core::Array<Token>::const_iterator it, const core::Array<Token::Type> &types) {
	if(!types.exists(it->type)) {
		it++;
		expected(it, types);
	}
}

void eat(core::Array<Token>::const_iterator &it, Token::Type type) {
	if((it++)->type != type) {
		expected(it, {type});
	}
}

bool isOperator(Token::Type type) {
	return !!(type & Token::isOperator);
}

uint assoc(Token::Type type) {
	return type & Token::associativityMask;
}




static core::Array<ASTDeclaration *> parseArgDecls(core::Array<Token>::const_iterator &begin, core::Array<Token>::const_iterator end);
static core::Array<ASTExpression *> parseArgs(core::Array<Token>::const_iterator &begin, core::Array<Token>::const_iterator end);

static ASTExpression *parseExpr(core::Array<Token>::const_iterator &begin, core::Array<Token>::const_iterator end);

static ASTExpression *parseSimpleExpr(core::Array<Token>::const_iterator &begin, core::Array<Token>::const_iterator end) {
	core::Array<Token>::const_iterator id = begin;
	switch((begin++)->type) {
		case Token::Identifier:
			if(begin->type == Token::Assign) {
				begin++;
				return new ASTAssignation(id->string, parseExpr(begin, end), (id + 1)->position);
			} else if(begin->type == Token::LeftPar) {
				return new ASTCall(id->string, parseArgs(begin, end), id->position);
			}
			return new ASTIdentifier(id->string, id->position);

		case Token::Integer:
			return new ASTLiteral(*id);

		case Token::LeftPar: {
			ASTExpression *expr = parseExpr(begin, end);
			if((begin++)->type != Token::RightPar) {
				delete expr;
				expected(begin, Token::RightPar);
			}
			return expr;
		} break;

		default:
		break;
	}
	expected(begin, {Token::Identifier, Token::Integer, Token::LeftPar});
	return 0;
}

ASTExpression *parseExpr(core::Array<Token>::const_iterator &begin, core::Array<Token>::const_iterator end) {
	ASTExpression *lhs = parseSimpleExpr(begin, end);
	Token::Type a = begin->type;

	if(!isOperator(a)) {
		return lhs;
	}
	begin++;
	ASTExpression *mhs = parseSimpleExpr(begin, end);

	for(;;) {
		Token::Type b = begin->type;

		if(isOperator(b)) {
			begin++;
		} else {
			return new ASTBinOp(lhs, mhs, a);
		}

		ASTExpression *rhs = parseSimpleExpr(begin, end);

		if(assoc(b) > assoc(a)) {
			mhs = new ASTBinOp(mhs, rhs, b);
		} else {
			lhs = new ASTBinOp(lhs, mhs, a);
			mhs = rhs;
			a = b;
		}
	}
	return 0;
}

static ASTDeclaration *parseDeclaration(core::Array<Token>::const_iterator &begin, core::Array<Token>::const_iterator end) {
	//eat(begin, Token::Var);
	if(begin->type == Token::Var) {
		begin++;
	}

	expect(begin, {Token::Identifier});
	core::String name = (begin++)->string;

	eat(begin, Token::Colon);

	expect(begin, {Token::Identifier});
	core::String type = (begin++)->string;

	if(begin->type == Token::Assign) {
		core::Array<Token>::const_iterator p = begin++;
		return new ASTDeclaration(name, type, p->position, parseExpr(begin, end));
	}
	return new ASTDeclaration(name, type, (begin - 1)->position);
}

static ASTInstruction *parseInstruction(core::Array<Token>::const_iterator &begin, core::Array<Token>::const_iterator end) {
	ASTInstruction *instr = 0;
	switch(begin->type) {
		case Token::Var:
			instr = parseDeclaration(begin, end);
		break;

		case Token::If: {
			begin++;
			expect(begin, {Token::LeftPar});
			ASTExpression *expr = parseExpr(begin, end);
			ASTInstruction *then = parseInstruction(begin, end);
			if(begin->type == Token::Else) {
				begin++;
				return new ASTBranch(expr, then, parseInstruction(begin, end));
			}
			return new ASTBranch(expr, then, 0);
		}	break;

		case Token::While: {
			begin++;
			expect(begin, {Token::LeftPar});
			ASTExpression *expr = parseExpr(begin, end);
			return new ASTLoop(expr, parseInstruction(begin, end));
		} break;

		case Token::LeftBrace: {
			begin++;
			core::Array<ASTInstruction *> instrs;
			while(begin->type != Token::RightBrace) {
				instrs.append(parseInstruction(begin, end));
			}
			begin++;
			return new ASTBlock(instrs);
		} break;

		case Token::Def: {
			begin++;
			expect(begin, {Token::Identifier});
			core::String name = begin->string;
			begin++;
			core::Array<ASTDeclaration *> args = parseArgDecls(begin, end);
			eat(begin, {Token::Assign});
			return new ASTFunctionDeclaration(name, args, parseInstruction(begin, end));
		} break;

		case Token::Return:
			begin++;
			instr = new ASTReturn(parseExpr(begin, end));
		break;

		default:
			instr = new ASTExprInstruction(parseExpr(begin, end));
		break;
	}
	if(instr) {
		eat(begin, Token::SemiColon);
		return instr;
	}
	expected(begin, {Token::Var, Token::While, Token::If, Token::LeftBrace});
	return 0;
}

core::Array<ASTDeclaration *> parseArgDecls(core::Array<Token>::const_iterator &begin, core::Array<Token>::const_iterator end) {
	eat(begin, {Token::LeftPar});
	core::Array<ASTDeclaration *> args;
	for(;;) {
		if(begin->type == Token::RightPar) {
			break;
		}
		args.append(parseDeclaration(begin, end));
		if(begin->type != Token::Coma) {
			break;
		}
		begin++;
	}
	eat(begin, {Token::RightPar});
	return args;
}

core::Array<ASTExpression *> parseArgs(core::Array<Token>::const_iterator &begin, core::Array<Token>::const_iterator end) {
	eat(begin, {Token::LeftPar});
	core::Array<ASTExpression *> args;
	for(;;) {
		if(begin->type == Token::RightPar) {
			break;
		}
		args.append(parseExpr(begin, end));
		if(begin->type != Token::Coma) {
			break;
		}
		begin++;
	}
	eat(begin, {Token::RightPar});
	return args;
}






Parser::Parser() {
}

ASTInstruction *Parser::parse(core::Array<Token>::const_iterator begin, core::Array<Token>::const_iterator end) const {
	core::Array<ASTInstruction *> instrs;
	while(begin != end && !(begin->type & Token::isEnd)) {
		instrs.append(parseInstruction(begin, end));
	}
	return instrs.size() == 1 ? instrs.first() : new ASTBlock(instrs);
}

}
}
