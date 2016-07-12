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
#ifndef N_SCRIPT_WTNODE_H
#define N_SCRIPT_WTNODE_H

#include <n/core/String.h>
#include <n/core/Array.h>
#include "DataType.h"
#include "FunctionTable.h"
#include "Scope.h"


namespace n {
namespace script {

struct WTNode : NonCopyable
{
	enum Type
	{
		Add,
		Substract,
		Multiply,
		Divide,

		Equals,
		NotEquals,
		LessThan,
		GreaterThan,

		Variable,

		Call,

		Return,

		Integer,
		New,

		Cast,

		Assignation,

		Loop,
		Branch,

		Expression,
		Block
	};

	WTNode(Type t) : type(t) {
	}

	const Type type;
};


struct WTExpression : public WTNode
{
	WTExpression(WTNode::Type t, DataType *vt, uint reg) : WTNode(t), expressionType(vt), registerIndex(reg) {
	}

	DataType *expressionType;

	uint registerIndex;
};

struct WTStatement : public WTNode
{
	WTStatement(WTNode::Type t) : WTNode(t) {
	}
};

struct WTVariable : public WTExpression
{
	WTVariable(const core::String &n, DataType *t, uint reg) : WTExpression(Variable, t, reg), name(n) {
	}

	core::String name;
};

struct WTFunction : NonCopyable
{
	WTFunction(const core::String &n, DataType *ret, WTStatement *bod = 0) : name(n), body(bod), returnType(ret) {
	}

	core::String name;
	WTStatement *body;
	core::Array<WTVariable *> args;
	DataType *returnType;

	Scope scope;

	core::String signature;

	void computeSignature() {
		signature = returnType->getName() + " " + name;
		for(WTVariable *a : args) {
			signature += " " + a->name;
		}
	}
};

}
}

#endif // N_SCRIPT_WTNODE_H
