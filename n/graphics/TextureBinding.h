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

#ifndef N_GRAPHICS_TEXTUREBINDING
#define N_GRAPHICS_TEXTUREBINDING

#include "Texture.h"

namespace n {
namespace graphics {
namespace internal {

class TextureBinding
{
	public:
		TextureBinding();

		TextureBinding &operator=(const Texture &t);

		void bind(uint slot) const;
		static void dirty();

	private:
		Texture tex;

		static uint active;
		static gl::GLuint *bindings;
};

}
}
}
#endif // N_GRAPHICS_TEXTUREBINDING

