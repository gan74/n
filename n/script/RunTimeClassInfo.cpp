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
#include "RuntimeClassInfo.h"

namespace n {
namespace script {

RuntimeFuncInfo RuntimeFuncInfo::error = RuntimeFuncInfo{0, 0, 0, 0, ""};

RuntimeClassInfo::RuntimeClassInfo(const core::String &n) : name(n) {
}

const RuntimeFuncInfo &RuntimeClassInfo::getMethod(uint index) const {
	for(const RuntimeFuncInfo &m : vtable) {
		if(m.index == index) {
			return m;
		}
	}
	return RuntimeFuncInfo::error;
}

const RuntimeFuncInfo &RuntimeClassInfo::getMethod(const core::String &n) const {
	for(const RuntimeFuncInfo &m : vtable) {
		if(m.name == n) {
			return m;
		}
	}
	return RuntimeFuncInfo::error;
}


}
}
