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
#include "RuntimeObject.h"
#include <n/core/Set.h>
#include <n/core/Map.h>
#include <cstring>

#include <iostream>

namespace n {
namespace script {

Machine::Machine() : argStackTop(new Primitive[1 << 16]) {
}

Primitive Machine::run(const char *mainName, uint memSize) {
	Primitive *memory = new Primitive[memSize];
	for(uint i = 0; i != memSize; i++) {
		memory[i].integer = 0;
	}
	Primitive ret;

	const FunctionInfo *main = 0;
	for(ClassInfo *i : classes) {
		main = i->find(mainName);
		if(main) {
			break;
		}
	}

	if(!main) {
		fatal("main() not found");
	}
	run(*main, memory, &ret);

	delete[] memory;
	return ret;
}

void Machine::run(FunctionInfo info, Primitive *mem, Primitive *ret) {

	Primitive *stackTop = mem + info.stackSize;
	const BytecodeInstruction *bcode = info.ptr;

	Primitive tmp;
	const FunctionInfo *function = 0;

	for(const BytecodeInstruction *i = bcode;; i++) {

		Primitive *m = mem + i->dst;

		switch(i->op) {

			case Bytecode::Nop:
			break;

			case Bytecode::Constants:
				fatal("Constants in bytecode");
			break;

			case Bytecode::FuncHead1:
				i++;
			case Bytecode::FuncHead2:
				//fatal("FuncHead in bytecode");
			break;

			case Bytecode::ClassHead:
				fatal("ClassHead in bytecode");
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

			case Bytecode::New:
				std::cout << "new " << getClass(info.constants->get(i->src[1])) << std::endl;
				m->object = new RuntimeObject(getClass(info.constants->get(i->src[1])));
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

			case Bytecode::CallVirtual:
				//fatal("callvirt");
				tmp = *stackTop = mem[i->src[0]];
				if(!tmp.object) {
					fatal("nullptr");
				}
				//std::cout << "->" << info.constants->get(i->src[1]) << std::endl;
				function = tmp.object->classInfo->find(info.constants->get(i->src[1]));
				if(!function) {
					fatal("function not found");
				}
				argStackTop -= function->args;
				memcpy(stackTop + 1, argStackTop, sizeof(Primitive) * function->args);
				run(*function, stackTop, m);
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
				return;

			case Bytecode::Exit:
				return;

		}
	}
}

void Machine::load(const BytecodeInstruction *bcode, const BytecodeInstruction *end) {

	struct FuncAlloc : NonCopyable
	{
		FuncAlloc(Machine *m) : begin(0), ma(m) {
		}

		void operator<<(const BytecodeInstruction *b) {
			if(begin && begin->op == Bytecode::FuncHead1) {
				ma->allocFunction(begin, b);
			}
			begin = b;
		}

		const BytecodeInstruction *begin;
		Machine *ma;

	} funcAllocator(this);


	for(const BytecodeInstruction *i = bcode; i != end; i++) {
		switch(i->op) {
			case Bytecode::FuncHead1: {
				funcAllocator << i;
			} break;

			case Bytecode::Constants: {
				funcAllocator << i;

				constPools << ConstantPool::createPool(i);
				i += i->udata;
			} break;

			case Bytecode::ClassHead:
				funcAllocator << i;

				if(constPools.isEmpty()) {
					fatal("No constant pool");
				}

				classes << new ClassInfo(constPools.last(), i->dst);
				std::cout << "class " << classes.last()->name << std::endl;
			break;

			default:
			break;
		}
	}

	funcAllocator << end;
}

void Machine::allocFunction(const BytecodeInstruction *begin, const BytecodeInstruction *end) {
	if(!begin || begin->op != Bytecode::FuncHead1) {
		return;
	}

	if(constPools.isEmpty()) {
		fatal("No constant pool");
	}
	uint classId = begin->dst;
	uint funcId = begin->src[0];
	uint stack = (begin + 1)->dst;
	uint args = (begin + 1)->src[0];

	BytecodeInstruction *funcMem = new BytecodeInstruction[end - begin];
	memcpy(funcMem, begin, (end - begin) * sizeof(BytecodeInstruction));
	functions << funcMem;

	ClassInfo *classInfo = getClass(constPools.last()->get(classId));

	if(!classInfo) {
		fatal("Invalid class");
	}

	classInfo->functions << FunctionInfo{funcMem, args, stack, constPools.last()->get(funcId), constPools.last()};

	std::cout << "def " << classInfo->name << "." << classInfo->functions.last().name << std::endl;
}

ClassInfo *Machine::getClass(const char *name) {
	for(ClassInfo *i : classes) {
		if(!strcmp(name, i->name)) {
			return i;
		}
	}
	return 0;
}

}
}
