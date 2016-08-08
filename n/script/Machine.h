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
#ifndef N_SCRIPT_MACHINE_H
#define N_SCRIPT_MACHINE_H

#include "ClassInfo.h"
#include "ConstantPool.h"

namespace n {
namespace script {

class Machine : NonCopyable
{
	public:
		Machine();

		void load(const BytecodeInstruction *bcode, const BytecodeInstruction *end);

		Primitive run(const char *mainName = "Int main", uint memSize = 1 << 16);

	private:
		void run(FunctionInfo info, Primitive *mem, Primitive *ret);
		void allocFunction(const BytecodeInstruction *begin, const BytecodeInstruction *end);

		ClassInfo *getClass(const char *name);

		Primitive *argStackTop;

		core::Array<ConstantPool *> constPools;
		core::Array<ClassInfo *> classes;
		core::Array<BytecodeInstruction *> functions;



};

}
}

#endif // N_SCRIPT_MACHINE_H