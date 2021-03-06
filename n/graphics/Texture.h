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

#ifndef N_GRAPHICS_TEXTURE_H
#define N_GRAPHICS_TEXTURE_H

#include "TextureBase.h"
#include "Image.h"
#include "GLContext.h"
#include "GL.h"

namespace n {
namespace graphics {

class ShaderBase;
class RenderableTexture;

class Texture : public TextureBase
{
	public:
		Texture(const Image &i, bool mip);
		Texture();
		~Texture();

		bool operator==(const Texture &t) const;
		bool operator!=(const Texture &t) const;
		bool operator<(const Texture &t) const;

		bool isNull() const {
			return !getHandle();
		}

		math::Vec2ui getSize() const {
			return image.getSize();
		}

		ImageFormat getFormat() const {
			return image.getFormat();
		}

		const Image &getImage() const {
			return image;
		}

		uint getMipmapLevel() const {
			return hasMipmaps() ? getMipmapLevelForSize(getSize()) : 1;
		}

		bool isMipCapable() const {
			return getSize().max() / float(getSize().min() + 1) < sqrt(2);
		}

		static uint getMipmapLevelForSize(const math::Vec2ui &size) {
			return 1 + floor(log2(size.max()));
		}

		static bool isHWSupported(ImageFormat format);

		bool synchronize(bool immediate = false) const;

	private:
		friend class FrameBuffer;
		friend class TextureArray;

		//Texture(const TextureBase &base, Image im);



		void upload() const;

		Image image;
};

class RenderableTexture : public Texture
{
	public:
		RenderableTexture();
		RenderableTexture(const math::Vec2ui &s, ImageFormat f = ImageFormat::RGBA8, bool mip = false);

		operator Texture() const;
};

}
}

#endif // N_GRAPHICS_TEXTURE_H
