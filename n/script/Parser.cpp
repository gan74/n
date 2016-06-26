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
#include "ast/ast.h"

namespace n {
namespace script {

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




static core::Array<ast::Declaration *> parseArgDecls(core::Array<Token>::const_iterator &begin, core::Array<Token>::const_iterator end);
static core::Array<ASTExpression *> parseArgs(core::Array<Token>::const_iterator &begin, core::Array<Token>::const_iterator end);

static ASTExpression *parseExpr(core::Array<Token>::const_iterator &begin, core::Array<Token>::const_iterator end);

static ASTExpression *parseSimpleExpr(core::Array<Token>::const_iterator &begin, core::Array<Token>::const_iterator end) {
	core::Array<Token>::const_iterator id = begin;
	switch((begin++)->type) {
		case Token::Identifier:
			if(begin->type == Token::Assign) {
				begin++;
				return new ast::Assignation(id->string, parseExpr(begin, end), (id + 1)->position);
			} else if(begin->type == Token::LeftPar) {
				return new ast::Call(id->string, parseArgs(begin, end), id->position);
			}
			return new ast::Identifier(id->string, id->position);

		case Token::Integer:
			return new ast::Literal(*id);

		case Token::LeftPar: {
			ASTExpression *expr = parseExpr(begin, end);
			eat(begin, Token::RightPar);
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
			return new ast::BinOp(lhs, mhs, a);
		}

		ASTExpression *rhs = parseSimpleExpr(begin, end);

		if(assoc(b) > assoc(a)) {
			mhs = new ast::BinOp(mhs, rhs, b);
		} else {
			lhs = new ast::BinOp(lhs, mhs, a);
			mhs = rhs;
			a = b;
		}
	}
	return 0;
}

static ast::Declaration *parseDeclaration(core::Array<Token>::const_iterator &begin, core::Array<Token>::const_iterator end) {
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
		return new ast::Declaration(name, type, p->position, parseExpr(begin, end));
	}
	return new ast::Declaration(name, type, (begin - 1)->position);
}

static ASTStatement *parseInstruction(core::Array<Token>::const_iterator &begin, core::Array<Token>::const_iterator end) {
	ASTStatement *instr = 0;
	switch(begin->type) {
		case Token::Var:
			instr = parseDeclaration(begin, end);
		break;

		case Token::If: {
			begin++;
			expect(begin, {Token::LeftPar});
			ASTExpression *expr = parseExpr(begin, end);
			ASTStatement *then = parseInstruction(begin, end);
			if(begin->type == Token::Else) {
				begin++;
				return new ast::Branch(expr, then, parseInstruction(begin, end));
			}
			return new ast::Branch(expr, then, 0);
		}	break;

		case Token::While: {
			begin++;
			expect(begin, {Token::LeftPar});
			ASTExpression *expr = parseExpr(begin, end);
			return new ast::Loop(expr, parseInstruction(begin, end));
		} break;

		case Token::LeftBrace: {
			begin++;
			core::Array<ASTStatement *> instrs;
			while(begin->type != Token::RightBrace) {
				instrs.append(parseInstruction(begin, end));
			}
			begin++;
			return new ast::Block(instrs);
		} break;

		case Token::Def: {
			begin++;
			expect(begin, {Token::Identifier});
			core::String name = begin->string;
			begin++;
			core::Array<ast::Declaration *> args = parseArgDecls(begin, end);
			eat(begin, Token::Colon);
			expect(begin, {Token::Identifier});
			core::String retType = begin->string;
			begin++;
			eat(begin, {Token::Assign});
			return new ast::FunctionDeclaration(name, retType, args, parseInstruction(begin, end));
		} break;

		case Token::Return:
			begin++;
			instr = new ast::Return(parseExpr(begin, end));
		break;

		default:
			instr = new ast::ExprInstr(parseExpr(begin, end));
		break;
	}
	if(instr) {
		eat(begin, Token::SemiColon);
		return instr;
	}
	expected(begin, {Token::Var, Token::While, Token::If, Token::LeftBrace});
	return 0;
}

core::Array<ast::Declaration *> parseArgDecls(core::Array<Token>::const_iterator &begin, core::Array<Token>::const_iterator end) {
	eat(begin, {Token::LeftPar});
	core::Array<ast::Declaration *> args;
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

ASTStatement *Parser::parse(core::Array<Token>::const_iterator begin, core::Array<Token>::const_iterator end) const {
	core::Array<ASTStatement *> instrs;
	while(begin != end && !(begin->type & Token::isEnd)) {
		instrs.append(parseInstruction(begin, end));
	}
	return instrs.size() == 1 ? instrs.first() : new ast::Block(instrs);
}

}
}
