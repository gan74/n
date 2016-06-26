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
#include "ClassBuilder.h"
#include "exceptions.h"
#include <n/script/wt/wt.h>

namespace n {
namespace script {

ClassBuilder::ClassBuilder(TypeSystem *t, FunctionTable *f, WTFunction *s) : current(s), typeSystem(t), methods(f) {
}

ClassBuilder::~ClassBuilder() {
	if(current) {
		current->stackSize = scope.getStackSize();
	}
}

FunctionTable &ClassBuilder::getMethods() {
	return *methods;
}

TypeSystem &ClassBuilder::getTypeSystem() {
	return *typeSystem;
}

WTFunction *ClassBuilder::getCurrentFunction() const {
	return current;
}

Scope &ClassBuilder::getScope() {
	return scope;
}

WTExpression *ClassBuilder::cast(WTExpression *expr, DataType *type, uint reg) const {
	if(!typeSystem->assign(type, expr->expressionType)) {
		throw ValidationErrorException("Assignation of incompatible types", TokenPosition());
	}
	return expr->expressionType == type ? expr : new wt::Cast(expr, type, reg);
}


}
}
