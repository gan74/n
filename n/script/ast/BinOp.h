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
#ifndef N_SCRIPT_ASTBINOP_H
#define N_SCRIPT_ASTBINOP_H

#include <n/script/ASTNode.h>
#include <n/script/Token.h>

namespace n {
namespace script {

namespace ast {

struct BinOp : public ASTExpression
{
	BinOp(ASTExpression *l, ASTExpression *r, Token::Type t) : ASTExpression(l->position), type(t), lhs(l), rhs(r) {
	}

	Token::Type type;
	const ASTExpression *lhs;
	const ASTExpression *rhs;

	virtual core::String toString() const override {
		return "(" + lhs->toString() + " "  + Token::getName(type) + " " + rhs->toString() + ")";
	}

	virtual WTExpression *toWorkTree(WTBuilder &builder, uint workReg) const override;
};

}

}
}

#endif // N_SCRIPT_ASTBINOP_H
