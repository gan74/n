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
#ifndef N_SCRIPT_VARIABLESTACK_H
#define N_SCRIPT_VARIABLESTACK_H

#include <n/core/Map.h>
#include <n/core/String.h>
#include <n/core/SmartPtr.h>
#include "TypeSystem.h"

namespace n {
namespace script {

class WTVariable;

class VariableStack : NonCopyable
{
	using VMap = core::Map<core::String, WTVariable *>;

	public:
		class Scope : NonCopyable
		{
			public:
				~Scope();

				WTVariable *declare(const core::String &name, DataType *type);
				uint alloc();

			private:
				Scope(VariableStack *stk);

				uint reg;
				VariableStack *stack;
				core::Array<VMap::iterator> scopedVars;
		};

		VariableStack();

		WTVariable *operator[](const core::String &name) const;

	private:
		uint reg;
		VMap variables;

};

}
}

#endif // N_SCRIPT_VARIABLESTACK_H
