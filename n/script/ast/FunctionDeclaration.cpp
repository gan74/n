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
#include <n/script/ClassBuilder.h>
#include <n/script/exceptions.h>
#include <n/script/wt/wt.h>

namespace n {
namespace script {
namespace ast {

WTStatement *ast::FunctionDeclaration::toWorkTree(ClassBuilder &builder, Scope &) const {
	WTFunction *function = builder.getMethods()[name];

	ClassBuilder b(&builder.getTypeSystem(), &builder.getMethods(), function);

	for(WTVariable *a : function->args) {
		b.getScope().declare(a);
	}
	function->body = body->toWorkTree(b);
	return 0;
}

void ast::FunctionDeclaration::lookupFunctions(ClassBuilder &builder) const {
	if(builder.getMethods()[name]) {
		throw ValidationErrorException("\"" + name + "\" has already been declared in this scope", position);
	}

	ClassBuilder b(&builder.getTypeSystem(), &builder.getMethods());

	core::Array<WTVariable *> arg;
	for(ast::Declaration *d : args) {
		if(d->value) {
			throw ValidationErrorException("Function parameter \"" + d->name + "\" should not have a value", position);
		}
		arg.append(b.getScope().declare(d->name, builder.getTypeSystem()[d->typeName]));
	}

	DataType *ret = builder.getTypeSystem()[retTypeName];
	if(!ret) {
		throw ValidationErrorException("\"" + retTypeName + "\" was not declared in this scope", position);
	}

	builder.getMethods().declare(name, arg, ret);
}
}
}
}
