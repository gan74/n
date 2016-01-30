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
#ifndef N_GRAPHICS_TILEDDEFERREDSHADINGRENDERER_H
#define N_GRAPHICS_TILEDDEFERREDSHADINGRENDERER_H

#include "GBufferRenderer.h"
#include "Light.h"
#include "ComputeShaderInstance.h"

namespace n {
namespace graphics {

class TiledDeferredShadingRenderer : public BufferedRenderer
{

	struct FrameData
	{
		void *gbufferData;
		math::Vec3 camPos;
		math::Matrix4<> inv;
		core::Array<DirectionalLight *> directionals;
	};

	struct DirectionalLightData
	{
		math::Vec3 color;
		int padding0;
		math::Vec3 forward;
		int padding1;

		static core::String toShader() {
			return
				"struct n_DirectionalLightData {"
					"vec3 color;"
					"int padding0;"
					"vec3 forward;"
					"int padding1;"
				"};";
		}
	};

	public:
		TiledDeferredShadingRenderer(GBufferRenderer *g, const math::Vec2ui &s = math::Vec2ui(0));
		~TiledDeferredShadingRenderer();

		virtual void *prepare() override;
		virtual void render(void *ptr) override;


	private:
		static Shader<ComputeShader> *createComputeShader();

		GBufferRenderer *gbuffer;
		ComputeShaderInstance *compute;
		UniformBuffer<DirectionalLightData> directionals;
};

}
}

#endif // N_GRAPHICS_TILEDDEFERREDSHADINGRENDERER_H
