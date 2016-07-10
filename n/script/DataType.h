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
#ifndef N_SCRIPT_DATATYPE_H
#define N_SCRIPT_DATATYPE_H

#include <n/core/String.h>
#include "Scope.h"
#include "FunctionTable.h"

namespace n {
namespace script {

class DataType : NonCopyable
{
	public:
		const core::String &getName() const;
		bool isObject() const;

		const FunctionTable &getMethods() const;
		FunctionTable &getMethods();

		const Scope &getScope() const;
		Scope &getScope();

		uint getIndex() const;

	private:
		friend class PrimitiveDataType;
		friend class ObjectDataType;

		DataType(const core::String &typeName, bool obj, uint i);

		const core::String name;
		const bool object;

		FunctionTable methods;
		Scope scope;

		uint index;
};

class PrimitiveDataType : public DataType
{
	public:
		PrimitiveDataType(const core::String &name, uint index, int coerceOrder = 0) : DataType(name, false, index), order(coerceOrder) {
		}

		int getCoercionOrder() const {
			return order;
		}

	private:
		int order;
};

class ObjectDataType : public DataType
{
	public:
		ObjectDataType(const core::String &name, uint index) : DataType(name, true, index) {
		}


	//private:
		/*FunctionTable methods;
		Scope scope;*/
};

class IntDataType : public PrimitiveDataType
{
	public:
		IntDataType() : PrimitiveDataType("Int", 0, 0) {
		}
};

class FloatDataType : public PrimitiveDataType
{
	public:
		FloatDataType() : PrimitiveDataType("Float", 1, 1) {
		}
};

}
}

#endif // N_SCRIPT_DATATYPE_H
