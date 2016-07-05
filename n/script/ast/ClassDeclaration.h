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
#ifndef N_SCRIPT_AST_CLASSDECLARATION_H
#define N_SCRIPT_AST_CLASSDECLARATION_H

#include <n/script/ASTNode.h>

namespace n {
namespace script {

namespace ast {

struct ClassDeclaration : public ASTStatement
{
	ClassDeclaration(const core::String &id, ASTStatement *bod, const TokenPosition &tk) : ASTStatement(tk), name(id), body(bod) {
	}

	const core::String name;
	const ASTStatement *body;

	virtual core::String toString() const override {
		return "class " + name + body->toString();
	}

	virtual WTStatement *toWorkTree(ClassBuilder &builder, Scope &) const override;
	virtual void lookupFunctions(ClassBuilder &builder) const;
	virtual void lookupTypes(TypeSystem &ts) const override;

	WTClass *getClass(ClassBuilder &builder) const;
};

}

}
}

#endif // N_SCRIPT_AST_CLASSDECLARATION_H
