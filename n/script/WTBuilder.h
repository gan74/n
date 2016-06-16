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
#ifndef N_SCRIPT_WTBUILDER_H
#define N_SCRIPT_WTBUILDER_H

#include "TypeSystem.h"
#include "WTNode.h"
#include "ValidationErrorException.h"

namespace n {
namespace script {


class WTBuilder : NonCopyable
{
	using VMap = core::Map<core::String, WTVariable *>;
	using FMap = core::Map<core::String, WTFunction *>;

	struct VarStackData
	{
		core::Array<typename core::Map<core::String, WTVariable *>::iterator> all;
		uint index;
	};

	struct FuncData
	{
		VMap vars;
		core::Array<VarStackData> varStack;
		WTFunction *func;
		uint maxReg;
	};

	public:
		WTBuilder();
		~WTBuilder();

		WTVariable *declareVar(const core::String &name, const core::String &typeName, TokenPosition tk = TokenPosition());
		WTVariable *getVar(const core::String &name, TokenPosition tk = TokenPosition()) const;

		WTFunction *declareFunc(const core::String &name, const core::Array<WTVariable *> &args, DataType *ret, TokenPosition tk = TokenPosition());
		WTFunction *getFunc(const core::String &name, TokenPosition tk = TokenPosition()) const;

		void enterScope();
		void leaveScope();

		void enterFunction(WTFunction *function = 0);
		void leaveFunction();

		uint allocRegister();

		WTFunction *getCurrentFunction() const;
		TypeSystem *getTypeSystem() const;

	private:
		FuncData &getLocalData();
		const FuncData &getLocalData() const;

		TypeSystem *types;

		core::Array<FuncData> funcStack;

		FMap funcMap;


};

}
}

#endif // WORKTREEBUILDER_H
