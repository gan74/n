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
#include "BytecodeAssembler.h"


namespace n {
namespace script {

using BCI = BytecodeInstruction;

BytecodeAssembler::BytecodeAssembler() : index(0) {
}

BytecodeAssembler::Label BytecodeAssembler::createLabel() {
	return Label(index);
}

BytecodeAssembler &BytecodeAssembler::ass(BytecodeInstruction i) {
	if(index == in.size()) {
		in << i;
	} else {
		in[index] = i;
	}
	index++;
	return *this;
}

BytecodeAssembler &BytecodeAssembler::operator<<(BytecodeInstruction i) {
	return ass(i);
}

BytecodeAssembler &BytecodeAssembler::operator<<(const BytecodeAssembler &a) {
	if(index == in.size()) {
		index += a.in.size();
	}
	in << a.in;
	return *this;
}

void BytecodeAssembler::seek(Label to) {
	index = to.index;
}

BytecodeAssembler::Label BytecodeAssembler::end() const {
	return Label(in.size());
}



BytecodeAssembler &BytecodeAssembler::nope() {
	return ass(BCI(Bytecode::Nope));
}

BytecodeAssembler &BytecodeAssembler::addI(RegisterType to, RegisterType a, RegisterType b) {
	return ass(BCI(Bytecode::AddI, to, a, b));
}

BytecodeAssembler &BytecodeAssembler::subI(RegisterType to, RegisterType a, RegisterType b) {
	return ass(BCI(Bytecode::SubI, to, a, b));
}

BytecodeAssembler &BytecodeAssembler::mulI(RegisterType to, RegisterType a, RegisterType b) {
	return ass(BCI(Bytecode::MulI, to, a, b));
}

BytecodeAssembler &BytecodeAssembler::divI(RegisterType to, RegisterType a, RegisterType b) {
	return ass(BCI(Bytecode::DivI, to, a, b));
}

BytecodeAssembler &BytecodeAssembler::addF(RegisterType to, RegisterType a, RegisterType b) {
	return ass(BCI(Bytecode::AddF, to, a, b));
}

BytecodeAssembler &BytecodeAssembler::subF(RegisterType to, RegisterType a, RegisterType b) {
	return ass(BCI(Bytecode::SubF, to, a, b));
}

BytecodeAssembler &BytecodeAssembler::mulF(RegisterType to, RegisterType a, RegisterType b) {
	return ass(BCI(Bytecode::MulF, to, a, b));
}

BytecodeAssembler &BytecodeAssembler::divF(RegisterType to, RegisterType a, RegisterType b) {
	return ass(BCI(Bytecode::DivF, to, a, b));
}


BytecodeAssembler &BytecodeAssembler::notI(RegisterType to, RegisterType from) {
	return ass(BCI(Bytecode::Not, to, from));
}

BytecodeAssembler &BytecodeAssembler::equals(RegisterType to, RegisterType a, RegisterType b) {
	return ass(BCI(Bytecode::Equals, to, a, b));
}

BytecodeAssembler &BytecodeAssembler::notEq(RegisterType to, RegisterType a, RegisterType b) {
	return ass(BCI(Bytecode::NotEq, to, a, b));
}

BytecodeAssembler &BytecodeAssembler::lessI(RegisterType to, RegisterType a, RegisterType b) {
	return ass(BCI(Bytecode::LessI, to, a, b));
}

BytecodeAssembler &BytecodeAssembler::greaterI(RegisterType to, RegisterType a, RegisterType b) {
	return ass(BCI(Bytecode::GreaterI, to, a, b));
}

BytecodeAssembler &BytecodeAssembler::lessF(RegisterType to, RegisterType a, RegisterType b) {
	return ass(BCI(Bytecode::LessF, to, a, b));
}

BytecodeAssembler &BytecodeAssembler::greaterF(RegisterType to, RegisterType a, RegisterType b) {
	return ass(BCI(Bytecode::GreaterF, to, a, b));
}

BytecodeAssembler &BytecodeAssembler::set(RegisterType to, int64 value) {
	BCI::DataType val = value;
	if(val != value) {
		#warning BytecodeAssembler fatal
		fatal("BytecodeAssembler : value too big");
	}
	return ass(BCI(Bytecode::SetI, to, val));
}

BytecodeAssembler &BytecodeAssembler::newObj(RegisterType to) {
	return ass(BCI(Bytecode::New, to));
}


BytecodeAssembler &BytecodeAssembler::toFloat(RegisterType to, RegisterType from) {
	return ass(BCI(Bytecode::ToFloat, to, from));
}

BytecodeAssembler &BytecodeAssembler::copy(RegisterType to, RegisterType from) {
	return ass(BCI(Bytecode::Copy, to, from));
}

BytecodeAssembler &BytecodeAssembler::jump(Label to) {
	return ass(BCI(Bytecode::JumpZ, 0, to.index - 1));
}

BytecodeAssembler &BytecodeAssembler::jumpNZ(RegisterType a, Label to) {
	return ass(BCI(Bytecode::JumpNZ, a, to.index - 1));
}

BytecodeAssembler &BytecodeAssembler::jumpZ(RegisterType a, Label to) {
	return ass(BCI(Bytecode::JumpZ, a, to.index - 1));
}

BytecodeAssembler &BytecodeAssembler::callVirtual(RegisterType to, RegisterType obj, RegisterType index) {
	return ass(BCI(Bytecode::InvokeVirtual, to, obj, index));
}

BytecodeAssembler &BytecodeAssembler::pushArg(RegisterType arg) {
	return ass(BCI(Bytecode::PushArg, arg));
}

BytecodeAssembler &BytecodeAssembler::ret(RegisterType from) {
	return ass(BCI(Bytecode::Ret, from));
}

BytecodeAssembler &BytecodeAssembler::retI(int64 value) {
	BCI::DataType val = value;
	if(val != value) {
		#warning BytecodeAssembler fatal
		fatal("BytecodeAssembler : value too big");
	}
	return ass(BCI(Bytecode::RetI, 0, val));
}

BytecodeAssembler &BytecodeAssembler::function(RegisterType classId, RegisterType index, RegisterType stack, RegisterType args) {
	ass(BCI(Bytecode::FuncHead1, classId, index));
	return ass(BCI(Bytecode::FuncHead2, stack, args));
}

BytecodeAssembler &BytecodeAssembler::classDecl(RegisterType classId) {
	return ass(BCI(Bytecode::ClassHead, classId));
}

BytecodeAssembler &BytecodeAssembler::exit() {
	return ass(BCI(Bytecode::Exit));
}

BytecodeAssembler &BytecodeAssembler::endFunc() {
	if(index - 1 < in.size()) {
		Bytecode op = in[index - 1].op;
		if(op == Bytecode::Exit || op == Bytecode::Ret || op == Bytecode::RetI) {
			return *this;
		}
	}
	return ass(BCI(Bytecode::Exit));
}

BytecodeAssembler &BytecodeAssembler::constants(const core::Array<core::String> &strings) {
	if(index != in.size()) {
		fatal("BytecodeAssembler not at end");
	}
	uint i = in.size();
	in << BCI(Bytecode::Constants, strings.size());
	for(const core::String &s : strings) {
		uint j = in.size();
		for(uint k = 0; k < s.size() + 1; k += sizeof(BCI)) {
			in << BCI(Bytecode::Nope);
		}
		memcpy(&in[j], s.data(), s.size());
	}
	in[i].udata = in.size() - i;

	index = in.size();
	return *this;
}


}
}
