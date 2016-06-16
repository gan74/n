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
#ifndef N_SCRIPT_FUNCTIONTABLE_H
#define N_SCRIPT_FUNCTIONTABLE_H

#include <n/core/String.h>
#include <n/core/Map.h>

namespace n {
namespace script {

class WTFunction;
class WTVariable;
class DataType;

class FunctionTable : NonCopyable
{
	using FMap = core::Map<core::String, WTFunction *>;

	public:
		FunctionTable();
		~FunctionTable();

		WTFunction *declare(const core::String &name, const core::Array<WTVariable *> &arguments, DataType *returnType);
		WTFunction *operator[](const core::String &name) const;

	private:
		FMap functions;


};

}
}

#endif // FUNCTIONTABLE_H
