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
#ifndef N_SCRIPT_TYPESYSTEM_H
#define N_SCRIPT_TYPESYSTEM_H

#include <n/core/Map.h>
#include <n/core/String.h>
#include "Tokenizer.h"
#include "DataType.h"

namespace n {
namespace script {

class TypeSystem : NonCopyable
{
	public:
		TypeSystem();

		DataType *operator[](const core::String &name) const;

		DataType *getType(const core::String &name) const;
		DataType *getIntType() const;
		DataType *getFloatType() const;

		DataType *addType(DataType *ty);
		const core::Map<core::String, DataType *> &getAll() const;

		bool assign(DataType *lhs, DataType *rhs);

		DataType *add(DataType *lhs, DataType *rhs);
		DataType *sub(DataType *lhs, DataType *rhs);
		DataType *mul(DataType *lhs, DataType *rhs);
		DataType *div(DataType *lhs, DataType *rhs);
		DataType *less(DataType *lhs, DataType *rhs);
		DataType *greater(DataType *lhs, DataType *rhs);

	private:
		DataType *primitiveOpCoerce(DataType *lhs, DataType *rhs);
		DataType *binOp(DataType *lhs, DataType *rhs);



		core::Map<core::String, DataType *> types;
};

}
}

#endif // N_SCRIPT_TYPESYSTEM_H
