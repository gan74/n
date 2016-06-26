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

#include "Return.h"
#include <n/script/ClassBuilder.h>
#include <n/script/ValidationErrorException.h>
#include <n/script/wt/wt.h>

namespace n {
namespace script {
namespace ast {

WTStatement *ast::Return::toWorkTree(ClassBuilder &builder, Scope &s) const {
	WTFunction *function = builder.getCurrentFunction();
	if(!function) {
		throw ValidationErrorException("return statement outside function", position);
	}
	#warning function may not have a return statement

	auto scope = s.nest();

	uint reg = scope.alloc();
	return new wt::Return(builder.cast(expression->toWorkTree(builder, scope, reg), function->returnType, reg));
}

void ast::Return::lookupFunctions(ClassBuilder &) const {
}

}
}
}
