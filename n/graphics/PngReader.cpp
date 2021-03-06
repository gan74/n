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

#ifdef N_USE_LODEPNG

#include "ImageLoader.h"
#include <dependencies/lodepng/lodepng.h>
#include <n/utils.h>
#include <n/defines.h>

namespace n {
namespace graphics {

class PngReader : public ImageLoader::AssetReader<PngReader, core::String>
{
	public:
		PngReader() : ImageLoader::AssetReader<PngReader, core::String>() {
		}

		ImageData *operator()(core::String name) override {
			N_LOG_PERF;
			std::vector<byte> image;
			uint32 width = 0;
			uint32 height = 0;
			uint err = lodepng::decode(image, width, height, name.toChar());
			if(err) {
				return 0;
			}
			return new ImageData(math::Vec2ui(width, height), ImageFormat::RGBA8, image.data());
		}
};

}
}

#endif
