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
#include "VariableStack.h"
#include "WTBuilder.h"

namespace n {
namespace script {

VariableStack::Scope::Scope(VariableStack *stk) : reg(stk->reg), stack(stk) {
}

VariableStack::Scope::~Scope() {
	if(stack->reg > reg) {
		stack->reg = reg;
	}
	for(const VMap::iterator &it : scopedVars) {
		stack->variables.remove(it);
	}
}

WTVariable *VariableStack::Scope::declare(const core::String &name, DataType *type) {
	if(stack->variables.exists(name)) {
		return 0;
	}
	WTVariable *v = new WTVariable(name, type, alloc());
	scopedVars << stack->variables.insert(name, v);
	return v;
}

uint VariableStack::Scope::alloc() {
	return stack->reg++;
}

VariableStack::VariableStack() : reg(0) {
}

WTVariable *VariableStack::operator[](const core::String &name) const {
	VMap::const_iterator it = variables.find(name);
	return it == variables.end() ? 0 : it->_2;
}

}
}
