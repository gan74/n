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
#include <n/script/exceptions.h>
#include <n/script/ClassBuilder.h>
#include <n/script/wt/wt.h>
#include "Identifier.h"

namespace n {
namespace script {
namespace ast {

WTFunction *Call::getFunction(ClassBuilder &builder, WTExpression *o) const {
	if(o) {
		DataType *t = o->expressionType;
		if(!t->isObject()) {
			throw ValidationErrorException("\"" + t->getName() + "\" is not an object.", position);
		}
		return static_cast<WTClass *>(t)->methods[name];
	}
	return builder.getFunctions()[name];
}

WTExpression *Call::toWorkTree(ClassBuilder &builder, Scope &s, uint workReg) const {
	WTExpression *obj = object->toWorkTree(builder, s, workReg);
	WTFunction *function = getFunction(builder, obj);

	if(!function) {
		throw ValidationErrorException("\"" + name + "\" was not declared in " + builder.getScopeName(), position);
	}
	if(function->args.size() != args.size()) {
		throw ValidationErrorException("Wrong number of argument (expected " + core::String(function->args.size()) + " got " + core::String(args.size()) + ")", position);
	}

	core::Array<WTExpression *> arg;
	for(uint i = 0; i != args.size(); i++) {
		auto scope = s.nest();
		uint reg = scope.alloc();
		WTExpression *ex = builder.cast(args[i]->toWorkTree(builder, scope, reg), function->args[i]->expressionType, reg, args[i]->position);
		arg << ex;
	}

	return new wt::Call(function, obj, arg, workReg);
}

}
}
}
