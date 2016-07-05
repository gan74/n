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

#include "Block.h"
#include <n/script/wt/wt.h>

namespace n {
namespace script {
namespace ast {

WTStatement *ast::Block::toWorkTree(ClassBuilder &builder, Scope &s) const {
	auto scope = s.nest();
	core::Array<WTStatement *> in;
	for(ASTStatement *i : instructions) {
		WTStatement *ii = i->toWorkTree(builder, scope);
		if(ii) {
			in.append(ii);
		}
	}
	return new wt::Block(in);
}

void ast::Block::lookupFunctions(ClassBuilder &builder) const {
	for(ASTStatement *i : instructions) {
		i->lookupFunctions(builder);
	}
}

void ast::Block::lookupTypes(TypeSystem &ts) const {
	for(ASTStatement *i : instructions) {
		i->lookupTypes(ts);
	}
}

}
}
}
