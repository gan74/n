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
#ifndef N_SCRIPT_AST_FUNCTIONDECLARATION_H
#define N_SCRIPT_AST_FUNCTIONDECLARATION_H

#include <n/script/ASTNode.h>
#include <n/core/Array.h>
#include "Declaration.h"

namespace n {
namespace script {

namespace ast {

struct FunctionDeclaration : public ASTInstruction
{
	FunctionDeclaration(const core::String &n, const core::String &tn, const core::Array<Declaration *> &a, ASTInstruction *bod) : ASTInstruction(bod->position), name(n), retTypeName(tn), args(a), body(bod) {
	}

	const core::String name;
	const core::String retTypeName;
	const core::Array<Declaration *> args;
	const ASTInstruction *body;

	virtual core::String toString() const override {
		core::String a;
		for(Declaration *d : args) {
			a += d->toString() + " ";
		}
		return "def " + name + "( " + a + ") = " + body->toString();
	}

	virtual WTInstruction *toWorkTree(ClassBuilder &builder, Scope &) const override;
	virtual void lookupFunctions(ClassBuilder &builder) const override;
};


}

}
}

#endif // N_SCRIPT_AST_FUNCTIONDECLARATION_H
