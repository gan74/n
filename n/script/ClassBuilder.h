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
#ifndef N_SCRIPT_CLASSBUILDER_H
#define N_SCRIPT_CLASSBUILDER_H

#include "TypeSystem.h"
#include "Scope.h"
#include "FunctionTable.h"
#include "WTNode.h"

namespace n {
namespace script {

class ClassBuilder
{
	public:
		ClassBuilder(TypeSystem *t, FunctionTable *f, WTFunction *s = 0);
		~ClassBuilder();

		FunctionTable &getMethods();
		TypeSystem &getTypeSystem();
		Scope &getScope();

		WTExpression *cast(WTExpression *expr, DataType *type, uint reg) const;

		WTFunction *getCurrentFunction() const;


	private:
		WTFunction *current;

		TypeSystem *typeSystem;
		FunctionTable *methods;
		Scope scope;
};

}
}
#endif // CLASSBUILDER_H
