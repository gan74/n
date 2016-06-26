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

#include "Call.h"
#include <n/script/ValidationErrorException.h>
#include <n/script/ClassBuilder.h>
#include <n/script/wt/wt.h>

namespace n {
namespace script {
namespace ast {

WTExpression *ast::Call::toWorkTree(ClassBuilder &builder, Scope &s, uint workReg) const {
	WTFunction *function = builder.getMethods()[name];
	if(function->args.size() != args.size()) {
		throw ValidationErrorException("Wrong number of argument (expected " + core::String(function->args.size()) + " got " + core::String(args.size()) + ")", position);
	}

	core::Array<WTExpression *> arg;
	for(uint i = 0; i != args.size(); i++) {
		auto scope = s.nest();
		uint reg = scope.alloc();
		WTExpression *ex = builder.cast(args[i]->toWorkTree(builder, scope, reg), function->args[i]->expressionType, reg);
		arg << ex;
	}

	return new wt::Call(function, arg, workReg);
}

}
}
}
