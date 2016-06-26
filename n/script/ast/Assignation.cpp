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

#include "Assignation.h"
#include <n/script/wt/wt.h>

namespace n {
namespace script {
namespace ast {

WTExpression *Assignation::toWorkTree(ClassBuilder &builder, Scope &scope, uint) const {
	WTVariable *v = scope[name];
	WTExpression *val = value->toWorkTree(builder, scope, v->registerIndex);
	return new wt::Assignation(v, builder.cast(val, v->expressionType, v->registerIndex));
}


}
}
}
