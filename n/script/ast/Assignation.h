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
#ifndef N_SCRIPT_ASTASSIGNATION_H
#define N_SCRIPT_ASTASSIGNATION_H

#include <n/script/ASTNode.h>

namespace n {
namespace script {

namespace ast {

struct Assignation : public ASTExpression
{
	Assignation(const core::String &id, ASTExpression *val, const TokenPosition &tk) : ASTExpression(tk), name(id), value(val) {
	}

	const core::String name;
	const ASTExpression *value;

	virtual core::String toString() const override {
		return name + " = " + value->toString();
	}

	virtual WTExpression *toWorkTree(WTBuilder &builder, uint) const override;
};

}

}
}

#endif // N_SCRIPT_ASTASSIGNATION_H
