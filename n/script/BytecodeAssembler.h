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
#ifndef N_SCRIPT_BYTECODEASSEMBLER_H
#define N_SCRIPT_BYTECODEASSEMBLER_H

#include "Bytecode.h"
#include <n/core/Array.h>

namespace n {
namespace script {

class BytecodeAssembler
{
	public:
		using RegisterType = BytecodeInstruction::RegisterType;
		using DataType = BytecodeInstruction::DataType;
		using UnsignedDataType = BytecodeInstruction::UnsignedDataType;

		class Label
		{

			private:
				friend class BytecodeAssembler;

				Label(UnsignedDataType i) : index(i) {
				}

				UnsignedDataType index;
		};

		BytecodeAssembler();

		Label createLabel();


		BytecodeAssembler &nope();

		BytecodeAssembler &addI(RegisterType to, RegisterType a, RegisterType b);
		BytecodeAssembler &subI(RegisterType to, RegisterType a, RegisterType b);
		BytecodeAssembler &mulI(RegisterType to, RegisterType a, RegisterType b);
		BytecodeAssembler &divI(RegisterType to, RegisterType a, RegisterType b);

		BytecodeAssembler &addF(RegisterType to, RegisterType a, RegisterType b);
		BytecodeAssembler &subF(RegisterType to, RegisterType a, RegisterType b);
		BytecodeAssembler &mulF(RegisterType to, RegisterType a, RegisterType b);
		BytecodeAssembler &divF(RegisterType to, RegisterType a, RegisterType b);


		BytecodeAssembler &notI(RegisterType to, RegisterType from);

		BytecodeAssembler &equals(RegisterType to, RegisterType a, RegisterType b);
		BytecodeAssembler &notEq(RegisterType to, RegisterType a, RegisterType b);

		BytecodeAssembler &lessI(RegisterType to, RegisterType a, RegisterType b);
		BytecodeAssembler &greaterI(RegisterType to, RegisterType a, RegisterType b);

		BytecodeAssembler &lessF(RegisterType to, RegisterType a, RegisterType b);
		BytecodeAssembler &greaterF(RegisterType to, RegisterType a, RegisterType b);

		BytecodeAssembler &copy(RegisterType to, RegisterType from);
		BytecodeAssembler &set(RegisterType to, int64 value);

		BytecodeAssembler &newObj(RegisterType to);

		BytecodeAssembler &toFloat(RegisterType to, RegisterType from);

		BytecodeAssembler &jump(Label to);
		BytecodeAssembler &jumpNZ(RegisterType a, Label to);
		BytecodeAssembler &jumpZ(RegisterType a, Label to);

		BytecodeAssembler &callStatic(RegisterType to, RegisterType classId, RegisterType index);
		BytecodeAssembler &callVirtual(RegisterType to, RegisterType obj, RegisterType index);

		BytecodeAssembler &pushArg(RegisterType arg);
		BytecodeAssembler &ret(RegisterType from);
		BytecodeAssembler &retI(int64 value);


		BytecodeAssembler &function(RegisterType classId, RegisterType index, RegisterType stack, RegisterType args);
		//BytecodeAssembler &classDecl();


		BytecodeAssembler &exit();
		BytecodeAssembler &endFunc();


		void seek(Label to);
		Label end() const;

		BytecodeAssembler &operator<<(BytecodeInstruction i);
		BytecodeAssembler &operator<<(const BytecodeAssembler &a);

		const core::Array<BytecodeInstruction> &getInstructions() const {
			return in;
		}

	private:
		BytecodeAssembler &ass(BytecodeInstruction i);

		core::Array<BytecodeInstruction> in;
		uint index;
};

}
}

#endif // N_SCRIPT_BYTECODEASSEMBLER_H
