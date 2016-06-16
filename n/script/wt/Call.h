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
#ifndef N_SCRIPT_WT_CALL_H
#define N_SCRIPT_WT_CALL_H

#include <n/script/WTNode.h>

namespace n {
namespace script {

class WTFunction;

namespace wt {

struct Call : public WTExpression
{
	Call(WTFunction *f, const core::Array<WTExpression *> &arg, uint reg) : WTExpression(WTNode::Call, f->returnType, reg), func(f), args(arg) {
	}

	WTFunction *func;
	core::Array<WTExpression *> args;
};

}
}
}

#endif // N_SCRIPT_WT_CALL_H
