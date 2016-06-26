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

#include "BinOp.h"
#include <n/script/ClassBuilder.h>
#include <n/script/ValidationErrorException.h>
#include <n/script/wt/wt.h>

namespace n {
namespace script {
namespace ast {

static DataType *notNull(DataType *t, const TokenPosition &p) {
	if(!t) {
		throw ValidationErrorException("Operation on incompatible types", p);
	}
	return t;
}

WTExpression *ast::BinOp::toWorkTree(ClassBuilder &builder, Scope &s, uint workReg) const {
	auto scope = s.nest();
	WTExpression *l = lhs->toWorkTree(builder, scope, workReg);

	scope = s.nest();
	uint rReg = scope.alloc();
	WTExpression *r = rhs->toWorkTree(builder, scope, rReg);

	DataType *t = 0;
	switch(type) {

		case Token::Plus:
			t = notNull(builder.getTypeSystem().add(l->expressionType, r->expressionType), position);
			return new wt::BinOp(WTNode::Add, builder.cast(l, t, workReg), builder.cast(r, t, rReg), t, workReg);

		case Token::Minus:
			t = notNull(builder.getTypeSystem().sub(l->expressionType, r->expressionType), position);
			return new wt::BinOp(WTNode::Substract, builder.cast(l, t, workReg), builder.cast(r, t, rReg), t, workReg);

		case Token::Multiply:
			t = notNull(builder.getTypeSystem().mul(l->expressionType, r->expressionType), position);
			return new wt::BinOp(WTNode::Multiply, builder.cast(l, t, workReg), builder.cast(r, t, rReg), t, workReg);

		case Token::Divide:
			t = notNull(builder.getTypeSystem().div(l->expressionType, r->expressionType), position);
			return new wt::BinOp(WTNode::Divide, builder.cast(l, t, workReg), builder.cast(r, t, rReg), t, workReg);

		case Token::LessThan:
			t = notNull(builder.getTypeSystem().less(l->expressionType, r->expressionType), position);
			return new wt::BinOp(WTNode::LessThan, builder.cast(l, t, workReg), builder.cast(r, t, rReg), t, workReg);

		case Token::GreaterThan:
			t = notNull(builder.getTypeSystem().greater(l->expressionType, r->expressionType), position);
			return new wt::BinOp(WTNode::GreaterThan, builder.cast(l, t, workReg), builder.cast(r, t, rReg), t, workReg);

		case Token::Equals:
			return new wt::BinOp(WTNode::Equals, l, r, builder.getTypeSystem().getIntType(), workReg);

		case Token::NotEquals:
			return new wt::BinOp(WTNode::NotEquals, l, r, builder.getTypeSystem().getIntType(), workReg);

		default:
		break;
	}
	delete l;
	delete r;
	throw ValidationErrorException("Unsupported operation", position);
	return 0;
}


}
}
}
