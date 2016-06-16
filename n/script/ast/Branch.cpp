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

#include "Branch.h"
#include <n/script/WTBuilder.h>
#include <n/script/wt/wt.h>

namespace n {
namespace script {
namespace ast {

WTInstruction *ast::Branch::toWorkTree(WTBuilder &builder) const {
	builder.enterScope();
	WTExpression *c = condition->toWorkTree(builder, builder.allocRegister());
	builder.leaveScope();

	builder.enterScope();
	WTInstruction *t = thenBody->toWorkTree(builder);
	builder.leaveScope();

	builder.enterScope();
	WTInstruction *e = elseBody ? elseBody->toWorkTree(builder) : 0;
	builder.leaveScope();

	return new wt::Branch(c, t, e);
}

void ast::Branch::lookupFunctions(WTBuilder &builder) const {
	thenBody->lookupFunctions(builder);
	if(elseBody) {
		elseBody->lookupFunctions(builder);
	}
}

}
}
}
