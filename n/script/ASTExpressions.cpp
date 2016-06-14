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
#include "WTNode.h"
#include "WTBuilder.h"
#include "ASTExpressions.h"

namespace n {
namespace script {

static WTVariableType *notNull(WTVariableType *t, const TokenPosition &p) {
	if(!t) {
		throw ValidationErrorException("Operation on incompatible types", p);
	}
	return t;
}

static WTExpression *cast(WTExpression *expr, WTVariableType *type, uint reg) {
	return expr->expressionType == type ? expr : new WTCast(expr, type, reg);
}

static WTExpression *cast(WTExpression *expr, WTVariableType *type, uint reg, WTBuilder &builder, TokenPosition position) {
	if(!builder.getTypeSystem()->assign(type, expr->expressionType)) {
		throw ValidationErrorException("Assignation of incompatible types", position);
	}
	return cast(expr, type, reg);
}

WTExpression *ASTIdentifier::toWorkTree(WTBuilder &builder, uint) const {
	return builder.getVar(name, position);
}

WTExpression *ASTLiteral::toWorkTree(WTBuilder &builder, uint workReg) const {
	switch(value.type) {
		case Token::Integer:
			return new WTInt(value.string.to<int64>(), builder.getTypeSystem()->getIntType(), workReg);

		default:
		break;
	}
	throw ValidationErrorException("Unsupported literal type", value.position);
	return 0;
}

WTExpression *ASTBinOp::toWorkTree(WTBuilder &builder, uint workReg) const {
	builder.enterScope();
	WTExpression *l = lhs->toWorkTree(builder, workReg);
	builder.leaveScope();

	builder.enterScope();
	uint rReg = builder.allocRegister();
	WTExpression *r = rhs->toWorkTree(builder, rReg);
	builder.leaveScope();

	WTVariableType *t = 0;
	switch(type) {

		case Token::Plus:
			t = notNull(builder.getTypeSystem()->add(l->expressionType, r->expressionType), position);
			return new WTBinOp(WTNode::Add, cast(l, t, workReg), cast(r, t, rReg), t, workReg);

		case Token::Minus:
			t = notNull(builder.getTypeSystem()->sub(l->expressionType, r->expressionType), position);
			return new WTBinOp(WTNode::Substract, cast(l, t, workReg), cast(r, t, rReg), t, workReg);

		case Token::Multiply:
			t = notNull(builder.getTypeSystem()->mul(l->expressionType, r->expressionType), position);
			return new WTBinOp(WTNode::Multiply, cast(l, t, workReg), cast(r, t, rReg), t, workReg);

		case Token::Divide:
			t = notNull(builder.getTypeSystem()->div(l->expressionType, r->expressionType), position);
			return new WTBinOp(WTNode::Divide, cast(l, t, workReg), cast(r, t, rReg), t, workReg);

		case Token::LessThan:
			t = notNull(builder.getTypeSystem()->less(l->expressionType, r->expressionType), position);
			return new WTBinOp(WTNode::LessThan, cast(l, t, workReg), cast(r, t, rReg), t, workReg);

		case Token::GreaterThan:
			t = notNull(builder.getTypeSystem()->greater(l->expressionType, r->expressionType), position);
			return new WTBinOp(WTNode::GreaterThan, cast(l, t, workReg), cast(r, t, rReg), t, workReg);

		case Token::Equals:
			return new WTBinOp(WTNode::Equals, l, r, builder.getTypeSystem()->getIntType(), workReg);

		case Token::NotEquals:
			return new WTBinOp(WTNode::NotEquals, l, r, builder.getTypeSystem()->getIntType(), workReg);

		default:
		break;
	}
	delete l;
	delete r;
	throw ValidationErrorException("Unsupported operation", position);
	return 0;
}

WTExpression *ASTAssignation::toWorkTree(WTBuilder &builder, uint) const {
	WTVariable *v = builder.getVar(name, position);
	WTExpression *val = value->toWorkTree(builder, v->registerIndex);
	return new WTAssignation(v, cast(val, v->expressionType, v->registerIndex, builder, position));
}

WTExpression *ASTCall::toWorkTree(WTBuilder &builder, uint workReg) const {
	WTFunction *function = builder.getFunc(name, position);
	if(function->args.size() != args.size()) {
		throw ValidationErrorException("Wrong number of argument (expected " + core::String(function->args.size()) + " got " + core::String(args.size()) + ")", position);
	}

	core::Array<WTExpression *> arg;
	for(uint i = 0; i != args.size(); i++) {
		builder.enterScope();
		uint reg = builder.allocRegister();
		WTExpression *ex = cast(args[i]->toWorkTree(builder, reg),
								function->args[i]->expressionType,
								reg,
								builder,
								position);
		arg << ex;
		builder.leaveScope();
	}

	return new WTCall(function, arg, workReg);
}


/*void ASTIdentifier::resolveFunctions(WTBuilder &builder) const {
}

void ASTLiteral::resolveFunctions(WTBuilder &builder) const {
}

void ASTBinOp::resolveFunctions(WTBuilder &builder) const {
}

void ASTAssignation::resolveFunctions(WTBuilder &builder) const {
}

void ASTCall::resolveFunctions(WTBuilder &builder) const {
}*/

}
}
