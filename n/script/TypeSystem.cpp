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
#include "TypeSystem.h"

namespace n {
namespace script {

TypeSystem::TypeSystem() {
	addType(new IntDataType());
	addType(new FloatDataType());
}

DataType *TypeSystem::operator[](const core::String &name) const {
	return getType(name);
}

void TypeSystem::addType(DataType *type) {
	types[type->getName()] = type;
}

DataType *TypeSystem::getType(const core::String &name) const {
	core::Map<core::String, DataType *>::const_iterator it = types.find(name);
	return it == types.end() ? 0 : it->_2;
}

DataType *TypeSystem::getIntType() const {
	return getType("Int");
}

DataType *TypeSystem::getFloatType() const {
	return getType("Float");
}

bool TypeSystem::assign(DataType *lhs, DataType *rhs) {
	if(!lhs->isObject() && !rhs->isObject()) {
		return primitiveOpCoerce(lhs, rhs) == lhs;
	}
	return false;
}

DataType *TypeSystem::primitiveOpCoerce(DataType *lhs, DataType *rhs) {
	PrimitiveDataType *l = static_cast<PrimitiveDataType *>(lhs);
	PrimitiveDataType *r = static_cast<PrimitiveDataType *>(rhs);
	return l->getCoercionOrder() < r->getCoercionOrder() ? r : l;
}

DataType *TypeSystem::binOp(DataType *lhs, DataType *rhs) {
	if(!lhs->isObject() && !rhs->isObject()) {
		return primitiveOpCoerce(lhs, rhs);
	}
	return 0;
}

DataType *TypeSystem::add(DataType *lhs, DataType *rhs) {
	return binOp(lhs, rhs);
}

DataType *TypeSystem::sub(DataType *lhs, DataType *rhs) {
	return binOp(lhs, rhs);
}

DataType *TypeSystem::mul(DataType *lhs, DataType *rhs) {
	return binOp(lhs, rhs);
}

DataType *TypeSystem::div(DataType *lhs, DataType *rhs) {
	return binOp(lhs, rhs);
}

DataType *TypeSystem::less(DataType *lhs, DataType *rhs) {
	return binOp(lhs, rhs);
}

DataType *TypeSystem::greater(DataType *lhs, DataType *rhs) {
	return binOp(lhs, rhs);
}


}
}
