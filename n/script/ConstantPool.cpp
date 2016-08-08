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
#include "ConstantPool.h"
#include <cstring>

namespace n {
namespace script {

ConstantPool::~ConstantPool() {
	delete[] strings;
	delete[] data;
}

ConstantPool::c_str ConstantPool::get(uint index) const {
	return strings[index];
}

ConstantPool::c_str ConstantPool::operator[](uint index) const {
	return get(index);
}


ConstantPool *ConstantPool::createPool(const BytecodeInstruction *instr) {
	if(instr->op != Bytecode::Constants) {
		fatal("ConstantPool *ConstantPool::createPool(BytecodeInstruction *) argument is not a constant pool.");
	}
	uint instrSize = instr->udata;
	uint strCount = instr->dst;

	char *data = new char[instrSize * sizeof(BytecodeInstruction)];
	memcpy(data, instr + 1, instrSize * sizeof(BytecodeInstruction));

	c_str *strings = new c_str[strCount];

	const char *start = data;
	for(uint i = 0; i != strCount; i++) {
		strings[i] = start;
		uint len = strlen(start) + 1;
		//std::cout << "constant: \t" << len << "\"" << start << "\"" << std::endl;
		if(len % sizeof(BytecodeInstruction)) {
			len += sizeof(BytecodeInstruction) - len % sizeof(BytecodeInstruction);
		}
		start = start + len;
	}
	return new ConstantPool(data, strings);

}


}
}
