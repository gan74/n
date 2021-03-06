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
		math::Vec3 camForward;
		math::Matrix4<> inv;
		core::Array<DirectionalLight *> directionals;
		core::Array<PointLight *> points;
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

	struct PointLightData
	{
		math::Vec3 color;
		float radius;
		math::Vec3 position;
		float scale;

		static core::String toShader() {
			return
				"struct n_PointLightData {"
					"vec3 color;"
					"float radius;"
					"vec3 position;"
					"float scale;"
				"};";
		}
	};

	public:
		enum Flags
		{
			None = 0x00000000,
			UseMinMaxDepth = 0x00000001,

			DebugLightCount = 0x00000002,
		};

		TiledDeferredShadingRenderer(GBufferRenderer *g, const math::Vec2ui &s = math::Vec2ui(0));
		~TiledDeferredShadingRenderer();


		void setFlags(Flags flags);

		virtual void *prepare() override;
		virtual void render(void *ptr) override;

	private:
		static Shader<ComputeShader> *createComputeShader(Flags flags = None);

		void setCompute(Shader<ComputeShader> *c);

		GBufferRenderer *gbuffer;
		ComputeShaderInstance *compute;
		UniformBuffer<DirectionalLightData> directionals;
		UniformBuffer<PointLightData> points;
};

}
}

#endif // N_GRAPHICS_TILEDDEFERREDSHADINGRENDERER_H
