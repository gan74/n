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

#include "FunctionDeclaration.h"
#include <n/script/WTBuilder.h>
#include <n/script/wt/wt.h>

namespace n {
namespace script {
namespace ast {

WTInstruction *ast::FunctionDeclaration::toWorkTree(WTBuilder &builder) const {
	WTFunction *function = builder.getFunc(name, position);
	builder.enterFunction(function);
	function->body = body->toWorkTree(builder);
	builder.leaveFunction();
	return 0;
}

void ast::FunctionDeclaration::lookupFunctions(WTBuilder &builder) const {
	builder.enterFunction();
	core::Array<WTVariable *> arg;
	for(ast::Declaration *d : args) {
		if(d->value) {
			throw ValidationErrorException("Function parameter \"" + d->name + "\" should not have a value", position);
		}
		arg.append(builder.declareVar(d->name, d->typeName, d->position));
	}
	builder.leaveFunction();

	DataType *ret = builder.getTypeSystem()->getType(retTypeName);
	if(!ret) {
		throw ValidationErrorException("\"" + retTypeName + "\" was not declared in this scope", position);
	}

	builder.declareFunc(name, arg, ret, position);
}
}
}
}
