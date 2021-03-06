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

#ifndef N_GRAPHICS_BUFFEREDRENDERER
#define N_GRAPHICS_BUFFEREDRENDERER


#include <n/utils.h>
#include "Renderer.h"
#include "FrameBufferPool.h"

namespace n {
namespace graphics {


class BufferedRenderer : public Renderer
{
	public:
		enum BufferKeepingPolicy
		{
			AlwaysKeep,
			DiscardAllowed
		};

		template<typename... Args>
		BufferedRenderer(const math::Vec2ui &s = math::Vec2ui(0), bool depth = true, Args... args) : BufferedRenderer(s, AlwaysKeep, depth, args...) {
		}

		template<typename... Args>
		BufferedRenderer(const math::Vec2ui &s, BufferKeepingPolicy keeps, bool depth = true, Args... args) : Renderer(), buffer(0), size(s.isNull() ? GLContext::getContext()->getViewport() : s), keepPolicy(keeps) {
			setupBufferFunc(depth, args...);
		}

		virtual ~BufferedRenderer() {
			keepPolicy = DiscardAllowed;
			discardBuffer();
		}

		const FrameBuffer &getFrameBuffer() const {
			const_cast<BufferedRenderer *>(this)->createBuffer();
			return *buffer;
		}

		void discardBuffer() {
			if(buffer && keepPolicy == DiscardAllowed) {
				GLContext::getContext()->getFrameBufferPool().add(buffer);
				buffer = 0;
			}
		}

		math::Vec2ui getSize() const {
			return size;
		}

	protected:
		template<typename... Args>
		void setBufferFormat(bool depth, Args... args) {
			setupBufferFunc(depth, args...);
			discardBuffer();
			createBuffer();
		}

	private:
		template<typename... Args>
		void setupBufferFunc(bool depth, Args... args) {
			std::tuple<bool, Args...> arg(depth, args...);
			createBufferFunc = core::Functor<FrameBuffer *()>([arg, this]() {
				return core::Functor<FrameBuffer *(bool, Args...)>([arg, this](bool d, Args... args){
					return GLContext::getContext()->getFrameBufferPool().get(getSize(), d, args...);
				})(arg);
			});
		}

		void createBuffer() {
			if(!buffer) {
				buffer = createBufferFunc();
			}
		}

		FrameBuffer *buffer;
		math::Vec2ui size;
		BufferKeepingPolicy keepPolicy;
		core::Functor<FrameBuffer *()> createBufferFunc;
};

}
}

#endif // N_GRAPHICS_BUFFEREDRENDERER

