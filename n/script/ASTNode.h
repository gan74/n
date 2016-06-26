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
#ifndef N_SCRIPT_ASTNODE_H
#define N_SCRIPT_ASTNODE_H

#include <n/core/String.h>
#include "ClassBuilder.h"
#include "Token.h"

namespace n {
namespace script {

class WTExpression;
class WTInstruction;

struct ASTNode : NonCopyable
{
	ASTNode(const TokenPosition &pos) : position(pos) {
	}

	virtual ~ASTNode() {
	}

	const TokenPosition position;

	virtual core::String toString() const = 0;
};

struct ASTExpression : public ASTNode
{
	ASTExpression(const TokenPosition &pos) : ASTNode(pos) {
	}

	virtual WTExpression *toWorkTree(ClassBuilder &, Scope &, uint) const = 0;
};

struct ASTInstruction : public ASTNode
{
	ASTInstruction(const TokenPosition &pos) : ASTNode(pos) {
	}

	WTInstruction *toWorkTree(ClassBuilder &builder) const {
		return toWorkTree(builder, builder.getScope());
	}

	virtual WTInstruction *toWorkTree(ClassBuilder &, Scope &) const = 0;
	virtual void lookupFunctions(ClassBuilder &) const = 0;
};

}
}

#endif // N_SCRIPT_ASTNODE_H
