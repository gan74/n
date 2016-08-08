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
#ifndef N_SCRIPT_CLASSINFO_H
#define N_SCRIPT_CLASSINFO_H

#include "Bytecode.h"
#include "FunctionInfo.h"
#include <n/core/Array.h>

namespace n {
namespace script {

struct ClassInfo : NonCopyable
{
	ClassInfo() : name(0), constants(0) {
	}

	ClassInfo(ConstantPool *pool, uint index) : name(pool->get(index)), constants(pool) {
	}

	const FunctionInfo *find(const char *funcName) const;

	const char *name;
	ConstantPool *constants;

	core::Array<FunctionInfo> functions;

};

}
}

#endif // N_SCRIPT_CLASSINFO_H
