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
#include "ClassDeclaration.h"
#include <n/script/exceptions.h>

namespace n {
namespace script {

namespace ast {

WTStatement *ClassDeclaration::toWorkTree(ClassBuilder &builder, Scope &) const {
	ClassBuilder b(builder, getClass(builder));
	return body->toWorkTree(b);
}

void ClassDeclaration::lookupFunctions(ClassBuilder &builder) const {
	ClassBuilder b(builder, getClass(builder));
	body->lookupFunctions(b);
}

void ClassDeclaration::lookupTypes(TypeSystem &ts) const {
	if(ts[name]) {
		throw ValidationErrorException("\"" + name + "\" has already been declared in this scope", position);
	}

	ts.addType(new ObjectDataType(name));
	body->lookupTypes(ts);
}

WTClass *ClassDeclaration::getClass(ClassBuilder &builder) const {
	DataType *type = builder.getTypeSystem()[name];
	if(!type) {
		throw ValidationErrorException("\"" + name + "\" was not declared in this scope", position);
	}
	if(!type->isObject()) {
		throw ValidationErrorException("\"" + name + "\" is not an object.", position);
	}
	return static_cast<WTClass *>(type);
}

}
}
}
