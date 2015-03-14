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

#ifndef N_GRAPHICS_CONTEXT_H
#define N_GRAPHICS_CONTEXT_H

#include <n/concurent/SynchronizedQueue.h>
#include <n/core/Functor.h>
#include <n/math/Matrix.h>

namespace n {
namespace graphics {

class ShaderCombinaison;
class FrameBuffer;

class GLContext
{
	public:
		enum HWInt
		{
			MaxFboAttachements = 0,
			Max = 1
		};

		static GLContext *getContext();


		math::Vec2ui getViewport() const;
		void setViewport(const math::Vec2ui &view);

		void addGLTask(const core::Functor<void()> &f);

		void setProjectionMatrix(const math::Matrix4<> &m);
		void setViewMatrix(const math::Matrix4<> &m);
		void setModelMatrix(const math::Matrix4<> &m);

		bool processTasks();
		void finishTasks();

		void setDebugEnabled(bool deb);
		static bool checkGLError();


		const ShaderCombinaison *getShader() const {
			return shader;
		}

		const FrameBuffer *getFrameBuffer() const {
			return frameBuffer;
		}


		int getHWInt(HWInt hw) {
			return hwInts[hw];
		}

	private:
		friend class ShaderCombinaison;
		friend class FrameBuffer;

		GLContext();
		~GLContext();

		concurent::SynchronizedQueue<core::Functor<void()>> tasks;

		math::Matrix4<> projection;
		math::Matrix4<> view;
		math::Matrix4<> model;

		const ShaderCombinaison *shader;
		const FrameBuffer *frameBuffer;

		int hwInts[Max];

		math::Vec2ui viewport;
};

}
}


#endif // N_GRAPHICS_CONTEXT_H
