/*******************************
Copyright (C) 2013-2016 gregoire ANGERAND

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

#include "types.h"
#include <n/core/String.h>

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

n::core::String demangle(const char* name) {
	int status = 0;
	char *d = abi::__cxa_demangle(name, 0, 0, &status);
	if(status) {
		return name;
	}
	n::core::String str(const_cast<const char *>(d));
	free(d);
	return str;
}

#else

n::core::String demangle(const char* name) {
	return name;
}

#endif

namespace n {
namespace details {
	uint typeId = 0;
}

core::String TypeData::name() const {
	return demangle(info->name());
}
}


