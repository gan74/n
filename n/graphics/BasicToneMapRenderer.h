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

#ifndef N_GRAPHICS_BASICTONEMAPRENDERER
#define N_GRAPHICS_BASICTONEMAPRENDERER

#include "BufferableRenderer.h"
#include "BufferedRenderer.h"
#include <n/core/Timer.h>

namespace n {
namespace graphics {

class BasicToneMapRenderer : public BufferableRenderer
{
	public:
		BasicToneMapRenderer(BufferedRenderer *c, uint s = 0);
		virtual ~BasicToneMapRenderer();

		virtual void *prepare() override;
		virtual void render(void *ptr) override;

		float getExposure() const {
			return exposure;
		}

		float getWhiteThreshold() const {
			return white;
		}

		void setExposure(float e) {
			exposure = e;
		}

		void setWhiteThreshold(float t) {
			white = t;
		}

		void setRange(float min, float max) {
			if(max < min) {
				std::swap(min, max);
			}
			range = math::Vec2(min, max);
		}

		void setDebugEnabled(bool d) {
			debug = d;
		}

		bool isDebugEnabled() const {
			return debug;
		}

	private:
		BufferedRenderer *child;
		FrameBuffer *luma;
		uint slot;
		float exposure;
		float white;
		math::Vec2 range;
		core::Timer timer;
		float adaptation;
		bool debug;
};

}
}


#endif // N_GRAPHICS_BASICTONEMAPRENDERER

