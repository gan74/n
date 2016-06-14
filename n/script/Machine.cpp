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
#include "Machine.h"

namespace n {
namespace script {

Machine::Machine() : argStackTop(new Primitive[1 << 16]) {
}

Primitive Machine::run(const BytecodeInstruction *bcode, uint memSize) {
	Primitive *memory = new Primitive[memSize];
	for(uint i = 0; i != memSize; i++) {
		memory[i].integer = 0;
	}
	Primitive ret;
	run(bcode, memory, &ret);

	delete[] memory;
	return ret;
}

void Machine::run(const BytecodeInstruction *bcode, Primitive *mem, Primitive *ret) {
	Primitive *stackTop = mem;

	for(const BytecodeInstruction *i = bcode;; i++) {

		Primitive *m = mem + i->dst;
		Primitive tmp;

		switch(i->op) {

			case Bytecode::Nope:
			break;

			case Bytecode::AddI:
				m->integer = mem[i->src[0]].integer + mem[i->src[1]].integer;
			break;

			case Bytecode::SubI:
				m->integer = mem[i->src[0]].integer - mem[i->src[1]].integer;
			break;

			case Bytecode::MulI:
				m->integer = mem[i->src[0]].integer * mem[i->src[1]].integer;
			break;

			case Bytecode::DivI:
				tmp = mem[i->src[1]];
				if(!tmp.integer) {
					fatal("divided by zero");
				}
				m->integer = mem[i->src[0]].integer / tmp.integer;;
			break;

			case Bytecode::AddF:
				m->real = mem[i->src[0]].real + mem[i->src[1]].real;
			break;

			case Bytecode::SubF:
				m->real = mem[i->src[0]].real - mem[i->src[1]].real;
			break;

			case Bytecode::MulF:
				m->real = mem[i->src[0]].real * mem[i->src[1]].real;
			break;

			case Bytecode::DivF:
				m->real = mem[i->src[0]].real / mem[i->src[1]].real;
			break;

			case Bytecode::LessI:
				m->integer = mem[i->src[0]].integer < mem[i->src[1]].integer;
			break;

			case Bytecode::GreaterI:
				m->integer = mem[i->src[0]].integer > mem[i->src[1]].integer;
			break;

			case Bytecode::LessF:
				m->real = mem[i->src[0]].real < mem[i->src[1]].real;
			break;

			case Bytecode::GreaterF:
				m->real = mem[i->src[0]].real > mem[i->src[1]].real;
			break;

			case Bytecode::Equals:
				m->integer = mem[i->src[0]].integer == mem[i->src[1]].integer;
			break;

			case Bytecode::NotEq:
				m->integer = mem[i->src[0]].integer != mem[i->src[1]].integer;
			break;

			case Bytecode::Not:
				m->integer = !mem[i->src[0]].integer;
			break;

			case Bytecode::Copy:
				*m = mem[i->src[0]];
			break;

			case Bytecode::SetI:
				m->integer = i->data;
			break;

			case Bytecode::SetF:
				m->real = i->fdata;
			break;

			case Bytecode::ToFloat:
				m->real = mem[i->src[0]].integer;
			break;

			case Bytecode::Jump:
				i = bcode + i->udata;
			break;

			case Bytecode::JumpZ:
				if(!m->integer) {
					i = bcode + i->udata;
				}
			break;

			case Bytecode::JumpNZ:
				if(m->integer) {
					i = bcode + i->udata;
				}
			break;

			case Bytecode::FuncHead1:
				i++;
			case Bytecode::FuncHead2:
				stackTop = mem + i->dst;
				argStackTop -= i->src[0];
				memcpy(mem, argStackTop, sizeof(Primitive) * i->src[0]);
			break;

			case Bytecode::Call:
				run(funcTable[i->udata], stackTop, m);
			break;

			case Bytecode::PushArg:
				*argStackTop = *m;
				argStackTop++;
			break;

			case Bytecode::RetI:
				ret->integer = i->data;
				return;

			case Bytecode::Ret:
				*ret = *m;
			case Bytecode::Exit:
				return;

		}
	}
}

void Machine::load(const BytecodeInstruction *bcode, const BytecodeInstruction *end) {
	for(const BytecodeInstruction *i = bcode; i != end; i++) {
		switch(i->op) {
			case Bytecode::FuncHead1:
				while(funcTable.size() <= i->udata) {
					funcTable << nullptr;
				}
				funcTable[i->udata] = i;
			break;


			default:
			break;
		}
	}
}

}
}
