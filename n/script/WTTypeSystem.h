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
#ifndef N_SCRIPT_WTTYPESYSTEM_H
#define N_SCRIPT_WTTYPESYSTEM_H

#include <n/core/Map.h>
#include <n/core/String.h>
#include "Tokenizer.h"
#include "WTVariableType.h"

namespace n {
namespace script {

class WTTypeSystem : NonCopyable
{
	public:
		WTTypeSystem();

		WTVariableType *getType(const core::String &name) const;
		WTVariableType *getIntType() const;
		WTVariableType *getFloatType() const;

		void addType(WTVariableType *type);

		bool assign(WTVariableType *lhs, WTVariableType *rhs);

		WTVariableType *add(WTVariableType *lhs, WTVariableType *rhs);
		WTVariableType *sub(WTVariableType *lhs, WTVariableType *rhs);
		WTVariableType *mul(WTVariableType *lhs, WTVariableType *rhs);
		WTVariableType *div(WTVariableType *lhs, WTVariableType *rhs);
		WTVariableType *less(WTVariableType *lhs, WTVariableType *rhs);
		WTVariableType *greater(WTVariableType *lhs, WTVariableType *rhs);

	private:
		WTVariableType *primitiveOpCoerce(WTVariableType *lhs, WTVariableType *rhs);
		WTVariableType *binOp(WTVariableType *lhs, WTVariableType *rhs);

		core::Map<core::String, WTVariableType *> types;
};

}
}

#endif // N_SCRIPT_WTTYPESYSTEM_H
