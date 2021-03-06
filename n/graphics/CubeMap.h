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

#ifndef N_GRAPHICS_CUBEMAP
#define N_GRAPHICS_CUBEMAP

#include "Texture.h"

namespace n {
namespace graphics {

class CubeMap : public TextureBase
{

	public:
		struct Cube
		{
			Image top;
			Image bottom;
			Image right;
			Image left;
			Image front;
			Image back;
		};


		CubeMap();
		CubeMap(const Cube &sides, bool mip);
		CubeMap(Image top, Image bottom, Image right, Image left, Image front, Image back, bool mip) : CubeMap(Cube{top, bottom, right, left, front, back}, mip) {
		}


		bool synchronize(bool sync = false) const;

		math::Vec2ui getSize() const {
			return cube.top.getSize();
		}

		ImageFormat getFormat() const {
			return cube.top.getFormat();
		}

	private:
		friend class CubeFrameBuffer;

		void upload() const;
		bool cubeLoading() const;
		bool cubeNull() const;

		Cube cube;


};

class RenderableCubeMap : public CubeMap
{
	public:
		RenderableCubeMap();
		RenderableCubeMap(const math::Vec2ui &s, ImageFormat f = ImageFormat::RGBA8, bool mip = false);

		operator CubeMap() const;
};

}
}

#endif // N_GRAPHICS_CUBEMAP

