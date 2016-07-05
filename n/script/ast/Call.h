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
#ifndef N_SCRIPT_AST_CALL_H
#define N_SCRIPT_AST_CALL_H

#include <n/script/ASTNode.h>
#include <n/core/Array.h>
#include "Identifier.h"

namespace n {
namespace script {

namespace ast {

struct Call : public ASTExpression
{
	Call(const core::String &id, ASTExpression *o, const core::Array<ASTExpression *> &args, const TokenPosition &tk) : ASTExpression(tk), name(id), object(o), args(args) {
	}

	const core::String name;
	ASTExpression *object;
	const core::Array<ASTExpression *> args;

	virtual core::String toString() const override {
		core::String a;
		for(ASTExpression *e : args) {
			a += e->toString() + " ";
		}
		return name + "( " + a + ")";
	}

	virtual WTExpression *toWorkTree(ClassBuilder &builder, Scope &s, uint workReg) const override;

	WTFunction *getFunction(ClassBuilder &builder, WTExpression *o) const;
};

}

}
}

#endif // N_SCRIPT_AST_CALL_H
