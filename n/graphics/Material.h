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

#ifndef N_GRAPHICS_MATERIAL
#define N_GRAPHICS_MATERIAL

#include "MaterialData.h"

namespace n {
namespace graphics {

class Material : public assets::Asset<MaterialData>
{
	public:
		Material() : assets::Asset<MaterialData>() {
		}

		Material(const MaterialData &i) : Material(new MaterialData(i)) {
		}

		Material(const MaterialData *i) : assets::Asset<MaterialData>(std::move(i)) {
		}

		Material(const assets::Asset<MaterialData> &t) : assets::Asset<MaterialData>(t) {
		}

		const MaterialData &getData() const;
		const MaterialData *operator->() const;
		bool synchronize(bool immediate = false) const;
};

static_assert(!IsThreadSafe<Material>::value, "n::graphics::Material should not be treated as thread-safe");

}
}

#endif // N_GRAPHICS_MATERIAL

