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
#include "Scope.h"
#include "WTNode.h"

namespace n {
namespace script {

Scope::Scope() : parent(0), top(this), reg(0) {
}

Scope::Scope(Scope *p) : parent(p), top(p->top), reg(p->reg) {
}

Scope::Scope(Scope &&s) : parent(s.parent), top(s.isTop() ? this : s.top), reg(s.reg) {
}

Scope &Scope::operator=(Scope &&s) {
	parent = s.parent;
	top = s.isTop() ? this : s.top;
	reg = s.reg;
	return *this;
}

Scope Scope::nest() {
	return this;
}

bool Scope::isTop() const {
	return top == this;
}

WTVariable *Scope::declare(const core::String &name, DataType *type) {
	Scope *s = this;
	while(s) {
		if(s->variables.exists(name)) {
			return 0;
		}
		s = s->parent;
	}

	WTVariable *var = new WTVariable(name, type, alloc());
	variables.insert(name, var);

	return var;
}

WTVariable *Scope::declare(WTVariable *var) {
	variables.insert(var->name, var);

	reg = var->registerIndex + 1;
	updateStackSize();

	return var;
}

WTVariable *Scope::operator[](const core::String &name) const {
	const Scope *s = this;
	while(s) {
		auto it = s->variables.find(name);
		if(it != s->variables.end()) {
			return it->_2;
		}
		s = s->parent;
	}
	return 0;
}

void Scope::updateStackSize() {
	stackSize = std::max(stackSize, reg);
	top->stackSize = std::max(top->stackSize, stackSize);
}

uint Scope::alloc() {
	uint r = reg++;
	updateStackSize();
	return r;
}


}
}
