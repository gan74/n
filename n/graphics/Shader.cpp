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

#include "Shader.h"
#include "ShaderProgram.h"
#include <n/core/Timer.h>
#include "MaterialData.h"

namespace n {
namespace graphics {

namespace internal {
	void rebindProgram() {
		ShaderProgram p = GLContext::getContext()->getShaderProgram();
		p.rebind();
	}
}

ShaderBase *ShaderBase::currents[3] = {0};

void ShaderBase::load(core::String src, uint vers) {
	N_LOG_PERF;
	source = parse(src, vers);
	version = vers;
}

void ShaderBase::compile() const {
	if(!handle) {
		handle = gl::createShader(type);
		const char *str = source.toChar();
		gl::shaderSource(handle, 1, &str, 0);
		if(!gl::compileShader(handle)) {
			throw ShaderCompilationException(gl::getShaderInfoLog(handle));
		}
	}
}

core::String ShaderBase::parse(core::String src, uint vers) {
	core::String libs[] = {
			// FRAGMENT
			"in vec3 n_Normal;"
			"in vec3 n_Tangent;"
			"in vec3 n_Binormal;"
			"in vec2 n_TexCoord;"

			"flat in uint n_InstanceID;"
			"\n#define n_BufferIndex n_InstanceID\n",

			// VERTEX
			"layout(location = 4) in uint n_DrawID;"
			"flat out uint n_InstanceID;"
			"\n#define n_BufferIndex (n_InstanceID = n_DrawID)\n",

			// GEOMETRY
			"flat out uint n_InstanceID;",

			// COMPUTE
			""
	};
	uint bufferSize = UniformBuffer<math::Matrix4<>>::getMaxSize();
	core::String ver = core::String("#version ") + vers + "\n#extension GL_ARB_bindless_texture : enable \n#define N_MAX_MATRIX_BUFFER_SIZE " + bufferSize + "\n";
	core::String model = "\n #define n_ModelMatrix n_ModelMatrices[n_BufferIndex] \n"
						 "uniform n_ModelMatrixBuffer { mat4 n_ModelMatrices[N_MAX_MATRIX_BUFFER_SIZE]; };";
	core::String material = "layout(std140) uniform n_MaterialBuffer { n_MaterialBufferData n_Materials[N_MAX_MATRIX_BUFFER_SIZE]; };"
							"\n #define n_RawMaterial n_Materials[n_BufferIndex]"
							"\n #define n_Material n_unpackMaterial(n_RawMaterial, n_TexCoord, normalize(n_Normal), normalize(n_Tangent), normalize(n_Binormal)) \n";
	core::String common =
		"uniform uint n_BaseInstance;"
		"layout(std140, row_major) uniform; "
		"layout (std140, row_major) buffer; "
		"const float pi = " + core::String(math::pi) + "; "
		"const float epsilon = 0.001; "
		"float sqr(float x) { return x * x; }  "
		"float saturate(float x) { return clamp(x, 0.0, 1.0); }"
		"vec2 saturate(vec2 x) { return clamp(x, vec2(0.0), vec2(1.0)); }"
		"vec3 saturate(vec3 x) { return clamp(x, vec3(0.0), vec3(1.0)); }"
		"vec4 saturate(vec4 x) { return clamp(x, vec4(0.0), vec4(1.0)); }"
		"vec2 sphereMap(vec3 U, vec3 N) { vec3 R = reflect(U, N); float m = -2.0 * sqrt(sqr(R.x) + sqr(R.y + 1.0) + sqr(R.z)); return R.xz / m + 0.5; }"
		"vec3 hemisphereSample(vec2 uv) {float phi = uv.y * 2.0 * pi; float cosTheta = 1.0 - uv.x; float sinTheta = sqrt(1.0 - cosTheta * cosTheta); return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta); }"

		"vec2 hammersley(uint i, uint N) { "
			"uint bits = i;"
			"bits = (bits << 16u) | (bits >> 16u);"
			"bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);"
			"bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);"
			"bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);"
			"bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);"
			"float VdC = float(bits) * 2.3283064365386963e-10;"
			"return vec2(float(i) / float(N), VdC);"
		"}"

		"struct n_GBufferData {"
			"vec4 color;"
			"vec3 normal;"
			"float roughness;"
			"float metallic;"
		"};"

		"vec4 n_packGBuffer0(n_GBufferData gbuffer) {"
			"return gbuffer.color;"
		"}"

		"vec4 n_packGBuffer1(n_GBufferData gbuffer) {"
			"return vec4(gbuffer.normal.xyz * 0.5 + 0.5, 0.0);"
		"}"

		"vec4 n_packGBuffer2(n_GBufferData gbuffer) {"
			"return vec4(gbuffer.roughness, gbuffer.metallic, 0.0, 1.0);"
		"}"

		"void n_packGBuffer(n_GBufferData gbuffer, out vec4 _0, out vec4 _1, out vec4 _2) {"
			"_0 = n_packGBuffer0(gbuffer);"
			"_1 = n_packGBuffer1(gbuffer);"
			"_2 = n_packGBuffer2(gbuffer);"
		"}"

		/*"n_GBufferData n_unpackGBuffer(sampler2D _0, sampler2D _1, sampler2D _2, vec2 uv) {"
			"n_GBufferData gb;"
			"gb.color = texture(_0, uv);"
			"gb.normal = texture(_1, uv).xyz * 2.0 - vec3(1.0);"
			"vec4 props = texture(_2, uv);"
			"gb.roughness = props.x + epsilon;"
			"gb.metallic = props.y;"
			"return gb;"
		"}"*/

		"n_GBufferData n_unpackGBuffer(sampler2D _0, sampler2D _1, sampler2D _2, ivec2 coord) {"
			"n_GBufferData gb;"
			"gb.color = texelFetch(_0, coord, 0);"
			"gb.normal = texelFetch(_1, coord, 0).xyz * 2.0 - vec3(1.0);"
			"vec4 props = texelFetch(_2, coord, 0);"
			"gb.roughness = props.x + epsilon;"
			"gb.metallic = props.y;"
			"return gb;"
		"}"

		+ MaterialBufferData::toShader() +

		"n_GBufferData n_unpackMaterial(n_MaterialBufferData mat, vec2 uv, vec3 normal, vec3 tangent, vec3 binormal) {"
			"n_GBufferData gb;"
			"gb.color = texture(mat.color, uv);"
			"vec4 props = texture(mat.properties, uv);"
			"vec2 n2 = props.xy * 2.0 - vec2(1.0);"
			//"gb.normal = normalize(n_Tangent) * n2.x + normalize(n_Binormal) * n2.y + normalize(n_Normal) * sqrt(1.0 - dot(n2, n2));"
			"gb.normal = tangent * n2.x + binormal * n2.y + normal * sqrt(1.0 - dot(n2, n2));"
			"gb.roughness = props.z;"
			"gb.metallic = props.w;"
			"return gb;"
		"}"

		"vec4 iblProbe(samplerCube cube, float invRoughnessPower, uint levels, vec3 d, float r) {"
			"r = pow(r, invRoughnessPower);"
			"return textureLod(cube, d, r * (levels - 1));"
		"}";


	uint vit = src.find("#version");
	if(vit != uint(-1)) {
		uint l = src.find("\n", vit);
		if(l != uint(-1)) {
			bool ok = true;
			uint v = src.subString(vit + 9).get<uint>([&]() { ok = false; });
			if(ok && v) {
				vers = v;
			} else {
				logs += "Unable to parse #version.\n";
			}
		}
		src.replace("#version", "//#version");
	}


	return ver + "\n" +  common + "\n" + libs[type] + "\n" + src.replaced("N_DECLARE_MODEL_MATRIX", model).replaced("N_DECLARE_MATERIAL_BUFFER", material);
}


ShaderBase::ShaderBase(ShaderType t) : type(t), version(0), handle(0) {
}

ShaderBase::~ShaderBase() {
	if(handle) {
		gl::Handle h = handle;
		GLContext::getContext()->addGLTask([=]() { gl::deleteShader(h); });
	}
}

const core::String &ShaderBase::getLogs() const {
	return logs;
}

bool ShaderBase::isValid() const {
	return handle && version;
}

uint ShaderBase::getVersion() const {
	return version;
}

bool ShaderBase::isCurrent() const {
	return currents[type] == this;
}

ShaderType ShaderBase::getType() const {
	return type;
}


}
}
