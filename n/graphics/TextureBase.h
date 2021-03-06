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

#ifndef N_GRAPHICS_TEXTURE_BASE_H
#define N_GRAPHICS_TEXTURE_BASE_H

#include <n/concurrent/SpinLock.h>
#include <n/core/SmartPtr.h>
#include "GL.h"
#include "GLContext.h"

namespace n {
namespace graphics {

class TextureBinding;

class TextureBase
{
	protected:
		class Data
		{
			public:
				Data(TextureType t) : type(t), handle(0), bindless(0), hasMips(false) {
				}

				~Data() {
					lock.trylock();
					lock.unlock();
					if(handle) {
						gl::Handle h = handle;
						GLContext::getContext()->addGLTask([=]() {
							gl::deleteTexture(h);
						});
					}
				}

				const TextureType type;
				concurrent::SpinLock lock;
				gl::Handle handle;
				uint64 bindless;
				bool hasMips;
		};


	public:
		TextureBase(TextureType type) : data(new Data(type)) {
		}

		bool hasMipmaps() const {
			return data->hasMips;
		}

		uint64 getBindlessId() const {
			return data->bindless;
		}

	protected:
		friend class ShaderInstanceBase;

		gl::Handle getHandle() const {
			return data->handle;
		}

		mutable core::SmartPtr<Data> data;
};

}
}


#endif // N_GRAPHICS_TEXTURE_BASE_H
