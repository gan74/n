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
#ifndef N_SCRIPT_AST_BLOCK_H
#define N_SCRIPT_AST_BLOCK_H

#include <n/script/ASTNode.h>
#include <n/core/Array.h>

namespace n {
namespace script {

namespace ast {

struct Block : public ASTStatement
{
	Block(const core::Array<ASTStatement *> &instrs) : ASTStatement(instrs.isEmpty() ? TokenPosition() : instrs.first()->position), instructions(instrs) {
	}

	const core::Array<ASTStatement *> instructions;

	virtual core::String toString() const override {
		core::String str;
		for(ASTStatement *i : instructions) {
			str += i->toString() + "\n";
		}
		return "{\n" + str + "}";
	}

	virtual WTStatement *toWorkTree(ClassBuilder &builder, Scope &s) const override;
	virtual void lookupFunctions(ClassBuilder &builder) const override;
	virtual void lookupTypes(TypeSystem &ts) const override;
};

}

}
}

#endif // N_SCRIPT_AST_BLOCK_H
