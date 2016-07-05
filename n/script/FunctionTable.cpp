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
#include "FunctionTable.h"
#include "WTNode.h"

namespace n {
namespace script {

FunctionTable::FunctionTable() {
}

FunctionTable::~FunctionTable() {
	/*for(const auto &p : functions) {
		delete p._2;
	}*/
}

WTFunction *FunctionTable::declare(const core::String &name, DataType *returnType) {
	if(functions.exists(name)) {
		return 0;
	}
	WTFunction *f = new WTFunction(name, returnType, functions.size());
	return functions[name] = f;
}

WTFunction *FunctionTable::operator[](const core::String &name) const {
	FMap::const_iterator it = functions.find(name);
	return it == functions.end() ? 0 : it->_2;
}

}
}
