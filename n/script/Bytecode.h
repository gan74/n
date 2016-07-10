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
#ifndef N_SCRIPT_BYTECODE_H
#define N_SCRIPT_BYTECODE_H

#include <n/types.h>
#include "RuntimeObject.h"

namespace n {
namespace script {

enum Bytecode : uint16
{
	Nope,

	AddI,
	SubI,
	MulI,
	DivI,

	AddF,
	SubF,
	MulF,
	DivF,

	LessI,
	GreaterI,

	LessF,
	GreaterF,

	Equals,
	NotEq,

	Not,

	Copy,

	SetI,
	SetF,

	New,

	ToFloat,

	Jump,
	JumpZ,
	JumpNZ,

	FuncHead1, //  func id
	FuncHead2, // arg num, stack size

	ClassHead,

	Invoke,

	PushArg,
	Ret,
	RetI,

	Exit

};

struct BytecodeInstruction
{
	using RegisterType = uint16;
	using DataType = int32;
	using FloatDataType = float;

	using UnsignedDataType = std::make_unsigned<DataType>::type;

	Bytecode op;
	RegisterType dst;

	union
	{
		RegisterType src[2];
		DataType data;
		UnsignedDataType udata;
		FloatDataType fdata;
	};

	static_assert(2 * sizeof(RegisterType) == sizeof(DataType), "Invalid BytecodeInstruction data type");
	static_assert(2 * sizeof(RegisterType) == sizeof(UnsignedDataType), "Invalid BytecodeInstruction data type");
	static_assert(2 * sizeof(RegisterType) == sizeof(FloatDataType), "Invalid BytecodeInstruction data type");

	BytecodeInstruction(Bytecode o, RegisterType a = 0, RegisterType b = 0, RegisterType c = 0) : op(o), dst(a), src{b, c} {
	}

	BytecodeInstruction(Bytecode o, RegisterType a, DataType d) : op(o), dst(a), data(d) {
	}

	BytecodeInstruction(Bytecode o, RegisterType a, UnsignedDataType d) : op(o), dst(a), udata(d) {
	}

	BytecodeInstruction(Bytecode o, RegisterType a, FloatDataType d) : op(o), dst(a), fdata(d) {
	}

};

union Primitive
{
	int64 integer;
	double real;
	RuntimeObject *object;
};

static_assert(sizeof(BytecodeInstruction) == 8, "BytecodeInstruction should be 64 bits");
static_assert(sizeof(Primitive) == 8, "Primitive should be 64 bits");
static_assert(sizeof(double) == sizeof(int64), "Primitive should be 64 bits");

}
}
#endif // N_SCRIPT_BYTECODE_H
