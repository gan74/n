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
#ifndef N_SCRIPT_RUNTIMEOBJECT_H
#define N_SCRIPT_RUNTIMEOBJECT_H

#include <n/types.h>

namespace n {
namespace script {

class ClassInfo;

/*class VTable
{
	public:
		using Entry = const BytecodeInstruction *;

		VTable(Entry *e = 0) : ptr(e) {
		}

		Entry operator[](uint index) const {
			return ptr[index];
		}

		const RuntimeClassInfo *getClassInfo() const {
			return reinterpret_cast<const RuntimeClassInfo *>(*(ptr - 1));
		}

		bool isNull() const {
			return !ptr || !*ptr;
		}

		Entry *getPointer() const {
			return ptr;
		}

	private:
		Entry *ptr;
};*/

class RuntimeObject
{
	public:
		RuntimeObject(ClassInfo *cl) : classInfo(cl) {
		}

		ClassInfo *classInfo;
};

}
}

#endif // N_SCRIPT_RUNTIMEOBJECT_H
