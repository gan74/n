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


#include "GL.h"
#include <n/core/Map.h>
#include <GL/glew.h>

//#define N_OGL_DOES_NOTHING

namespace n {
namespace graphics {
namespace gl {

bool isSampler(UniformType type) {
	static constexpr GLenum samplers[] = {
	GL_SAMPLER_1D,
	GL_SAMPLER_2D,
	GL_SAMPLER_3D,
	GL_SAMPLER_CUBE,
	GL_SAMPLER_1D_SHADOW,
	GL_SAMPLER_2D_SHADOW,
	GL_SAMPLER_1D_ARRAY,
	GL_SAMPLER_2D_ARRAY,
	GL_SAMPLER_1D_ARRAY_SHADOW,
	GL_SAMPLER_2D_ARRAY_SHADOW,
	GL_SAMPLER_2D_MULTISAMPLE,
	GL_SAMPLER_2D_MULTISAMPLE_ARRAY,
	GL_SAMPLER_CUBE_SHADOW,
	GL_SAMPLER_BUFFER,
	GL_SAMPLER_2D_RECT,
	GL_SAMPLER_2D_RECT_SHADOW,
	GL_INT_SAMPLER_1D,
	GL_INT_SAMPLER_2D,
	GL_INT_SAMPLER_3D,
	GL_INT_SAMPLER_CUBE,
	GL_INT_SAMPLER_1D_ARRAY,
	GL_INT_SAMPLER_2D_ARRAY,
	GL_INT_SAMPLER_2D_MULTISAMPLE,
	GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
	GL_INT_SAMPLER_BUFFER,
	GL_INT_SAMPLER_2D_RECT,
	GL_UNSIGNED_INT_SAMPLER_1D,
	GL_UNSIGNED_INT_SAMPLER_2D,
	GL_UNSIGNED_INT_SAMPLER_3D,
	GL_UNSIGNED_INT_SAMPLER_CUBE,
	GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,
	GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,
	GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE,
	GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
	GL_UNSIGNED_INT_SAMPLER_BUFFER,
	GL_UNSIGNED_INT_SAMPLER_2D_RECT};
	for(uint i = 0; i != sizeof(samplers) / sizeof(samplers[0]); i++) {
		if(type == samplers[i]) {
			return true;
		}
	}
	return false;
}

TextureFormat getTextureFormat(ImageFormat format) {
	switch(format) {
		case ImageFormat::Depth32:
			return TextureFormat(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32, GL_FLOAT);
		break;
		case ImageFormat::F32:
			return TextureFormat(GL_RED, GL_R32F, GL_FLOAT);
		break;
		case ImageFormat::RGBA32F:
			return TextureFormat(GL_RGBA, GL_RGBA32F, GL_FLOAT);
		break;
		case ImageFormat::RG16:
			return TextureFormat(GL_RG, GL_RG16, GL_UNSIGNED_SHORT);
		break;
		case ImageFormat::RG16F:
			return TextureFormat(GL_RG, GL_RG16F, GL_UNSIGNED_SHORT);
		break;
		case ImageFormat::RG32F:
			return TextureFormat(GL_RG, GL_RG32F, GL_FLOAT);
		break;
		case ImageFormat::RGBA8:
			return TextureFormat(GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE);
		break;
		case ImageFormat::RGB8:
			return TextureFormat(GL_RGB, GL_RGB8, GL_UNSIGNED_BYTE);
		break;
		case ImageFormat::RGBA16:
			return TextureFormat(GL_RGBA, GL_RGBA16, GL_UNSIGNED_SHORT);
		break;
		case ImageFormat::RGBA16F:
			return TextureFormat(GL_RGBA, GL_RGBA16F, GL_UNSIGNED_SHORT);
		break;
		default:
			return fatal("Unsuported texture format.");
		break;
	}
}

bool isHWSupported(ImageFormat format) {
	if(format == ImageFormat::RGB10A2) {
		return false;
	}
	static core::Map<ImageFormat, bool> support;
	core::Map<ImageFormat, bool>::const_iterator it = support.find(format);
	if(it == support.end()) {
		GLint s = 0;
		glGetInternalformativ(GL_TEXTURE_2D, getTextureFormat(format).internalFormat, GL_INTERNALFORMAT_SUPPORTED, sizeof(GLint), &s);
		return support[format] = (s == GL_TRUE);
	}
	return (*it)._2;
}

bool isBindlessHandle(UniformType t) {
	return t == GL_UNSIGNED_INT_VEC2;
}

#ifdef N_OGL_DOES_NOTHING

void genDummies(uint c, Handle *x) {
	for(uint i = 0; i != c; i++) {
		x[i] = 1;
	}
}

void genTextures(uint c, Handle *t) { genDummies(c, t); }
void genBuffers(uint c, Handle *t) { genDummies(c, t); }
void genVertexArrays(uint c, Handle *t) { genDummies(c, t); }
void genFramebuffers(uint c, Handle *t) { genDummies(c, t); }
void deleteTextures(uint, const Handle *) {}
void deleteBuffers(uint, const Handle *) {}
void deleteVertexArrays(uint, const Handle *) {}
void deleteFramebuffers(uint, const Handle *) {}
void bindTexture(TextureType, Handle) {}
void activeTexture(uint) {}
void bindTextureUnit(uint, Handle) {}
void bindSampler(uint, Handle) {}
void bindBuffer(BufferBinding, Handle) {}
void bindBufferBase(BufferBinding, uint, Handle) {}
void bufferData(BufferBinding, uint, const void *, BufferAlloc) {}
void bufferSubData(BufferBinding, uint, uint, const void *) {}
void bindVertexArray(Handle) {}
void vertexAttribPointer(uint, uint, Type, bool, uint, const void *) {}
void enableVertexAttribArray(uint) {}
void framebufferTexture2D(FrameBufferType, Attachment, TextureType, Handle, uint) {}
void drawBuffers(uint, const Attachment *) {}
FrameBufferStatus checkFramebufferStatus(FrameBufferType) { return FboOk; }
void bindFramebuffer(FrameBufferType, Handle) {}
void readBuffer(Attachment) {}
void clear(BitField) {}
void blitFramebuffer(int, int, int, int, int, int, int, int, BitField, Filter) {}
Handle createShader(ShaderType) { return 1; }
void shaderSource(Handle, uint, const char **, const uint *) {}
void compileShader(Handle) {}
void getShaderiv(Handle, ShaderParam, int *i) { *i = 1; }
void getShaderInfoLog(Handle, uint, int *, char *) {}
void deleteShader(Handle) {}
Handle createProgram() { return 1; }
void deleteProgram(Handle) {}
void useProgram(Handle) {}
void attachShader(Handle, Handle) {}
void linkProgram(Handle) {}
void getProgramiv(Handle, ShaderParam p, int *i) { *i = p == gl::LinkStatus; }
void getProgramInfoLog(Handle, uint, int *, char *) {}
void getActiveUniform(Handle, uint, uint, uint *, int *, UniformType *, char *) {}
void getActiveUniformBlockName(Handle, uint, uint, int *, char *) {}
int getUniformLocation(Handle, const char *) { return InvalidIndex; }
uint getUniformBlockIndex(Handle, const char *) { return 0; }
void uniformBlockBinding(Handle, uint, uint) {}
void viewport(int, int, int, int) {}
void drawElementsInstancedBaseVertex(PrimitiveType, uint, Type, void *, uint, uint) {}
void enable(Feature) {}
void disable(Feature) {}
void depthFunc(DepthMode) {}
void blendFunc(BlendMode) {}
void depthMask(bool) {}
void cullFace(CullMode) {}
void colorMask(bool, bool, bool, bool) {}
void programUniform1iv(Handle, int, uint, const int *) {}
void programUniform1uiv(Handle, int, uint, const uint *) {}
void programUniform1fv(Handle, int, uint, const float *) {}
void programUniform2iv(Handle, int, uint, const int *) {}
void programUniform3iv(Handle, int, uint, const int *) {}
void programUniform2fv(Handle, int, uint, const float *) {}
void programUniform3fv(Handle, int, uint, const float *) {}
void programUniform4fv(Handle, int, uint, const float *) {}
void programUniformMatrix2fv(Handle, int, uint, bool, const float *) {}
void programUniformMatrix3fv(Handle, int, uint, bool, const float *) {}
void programUniformMatrix4fv(Handle, int, uint, bool, const float *) {}
void programUniformHandleui64(Handle, int, uint64) {}
void texImage2D(TextureType, int, uint, uint, int, TextureFormat, const void *) {}
void generateMipmap(TextureType) {}
uint64 getTextureSamplerHandle(Handle) { return 1; }
void makeTextureHandleResident(uint64) {}

#else

GLenum textureType[] = {GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP};
GLenum bufferType[] = {GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_UNIFORM_BUFFER};
GLenum bufferUsage[] = {GL_STREAM_DRAW, GL_STATIC_DRAW, GL_DYNAMIC_DRAW};
GLenum framebufferType[] = {GL_FRAMEBUFFER};
GLenum dataType[] = {GL_NONE, GL_FLOAT, GL_INT, GL_UNSIGNED_INT, GL_SHORT, GL_UNSIGNED_SHORT, GL_BYTE, GL_UNSIGNED_BYTE, GL_DOUBLE};
GLenum textureFilter[] = {GL_NEAREST, GL_LINEAR};
GLenum shaderType[] = {GL_FRAGMENT_SHADER, GL_VERTEX_SHADER, GL_GEOMETRY_SHADER};
GLenum shaderParam[] = {GL_COMPILE_STATUS, GL_INFO_LOG_LENGTH, GL_LINK_STATUS, GL_ACTIVE_UNIFORMS};
GLenum features[] = {GL_DEPTH_TEST, GL_DEPTH_CLAMP, GL_CULL_FACE, GL_BLEND};
GLenum primitiveMode[] = {GL_TRIANGLES};
GLenum depthMode[] = {GL_LEQUAL, GL_GEQUAL, GL_ALWAYS};
GLenum blendModeSrc[] = {GL_ONE, GL_ONE, GL_SRC_ALPHA};
GLenum blendModeDst[] = {GL_ZERO, GL_ONE, GL_ONE_MINUS_SRC_ALPHA};
GLenum cullMode[] = {GL_NONE, GL_BACK, GL_FRONT};


GLenum toGLAttachement(Attachment att) {
	if(att == DepthAtt) {
		return GL_DEPTH_ATTACHMENT;
	}
	if(att == NoAtt) {
		return GL_NONE;
	}
	return GL_COLOR_ATTACHMENT0 + att;
}

void genTextures(uint count, Handle *textures) {
	glGenTextures(count, textures);
}

void genBuffers(uint count, Handle *buffers) {
	glGenBuffers(count, buffers);
}

void genVertexArrays(uint count, Handle *arrays) {
	glGenVertexArrays(count, arrays);
}

void genFramebuffers(uint count, Handle *framebuffers) {
	glGenFramebuffers(count, framebuffers);
}

void deleteTextures(uint count, const Handle *textures) {
	glDeleteTextures(count, textures);
}

void deleteBuffers(uint count, const Handle *buffers) {
	glDeleteBuffers(count, buffers);
}

void deleteVertexArrays(uint count, const Handle *arrays) {
	glDeleteVertexArrays(count, arrays);
}

void deleteFramebuffers(uint count, const Handle *buffers) {
	glDeleteFramebuffers(count, buffers);
}

void bindTexture(TextureType type, Handle tex) {
	glBindTexture(textureType[type], tex);
}

void activeTexture(uint slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
}

void bindTextureUnit(uint slot, Handle tex) {
	fatal("lelelle");
}

void bindSampler(uint slot, Handle sampler) {
	glBindSampler(slot, sampler);
}

void bindBuffer(BufferTarget target, Handle buffer) {
	glBindBuffer(bufferType[target], buffer);
}

void bindBufferBase(BufferTarget target, uint index, Handle buffer) {
	glBindBufferBase(bufferType[target], index, buffer);
}

void bufferData(BufferTarget target, uint size, const void *data, BufferAlloc usage) {
	glBufferData(bufferType[target], size, data, bufferUsage[usage]);
}

void bufferSubData(BufferTarget target, uint offset, uint size, const void *data) {
	glBufferSubData(bufferType[target], offset, size, data);
}

void bindVertexArray(Handle array) {
	glBindVertexArray(array);
}

void vertexAttribPointer(uint index, uint size, Type type, bool norm, uint stride, const void *ptr) {
	glVertexAttribPointer(index, size, dataType[type], norm, stride, ptr);

}
void enableVertexAttribArray(uint index) {
	glEnableVertexAttribArray(index);
}
void framebufferTexture2D(FrameBufferType target, Attachment attachement, TextureType texture, Handle handle, uint level) {
	glFramebufferTexture2D(framebufferType[target], toGLAttachement(attachement), texture, handle, level);
}

void drawBuffers(uint count, const Attachment *buffers) {
	GLenum *att = new GLenum[count];
	for(uint i = 0; i != count; i++) {
		att[count] = toGLAttachement(buffers[i]);
	}
	glDrawBuffers(count, att);
	delete[] att;
}

FrameBufferStatus checkFramebufferStatus(FrameBufferType framebuffer) {
	#warning checkFramebufferStatus does nothing
	return FboOk;
}


void bindFramebuffer(FrameBufferType target, Handle fbo) {
	glBindFramebuffer(framebufferType[target], fbo);
}

void readBuffer(Attachment attachment) {
	glReadBuffer(toGLAttachement(attachment));
}

void clear(BitField buffers) {
	GLbitfield b = (buffers & ColorBit ? GL_COLOR_BUFFER_BIT : 0) | (buffers & DepthBit ? GL_DEPTH_BUFFER_BIT : 0);
	glClear(b);
}

void blitFramebuffer(int srcX0, int srcY0, int srcX1, int srcY1, int dstX0, int dstY0, int dstX1, int dstY1, BitField mask, Filter filter) {
	GLbitfield b = (mask & ColorBit ? GL_COLOR_BUFFER_BIT : 0) | (mask & DepthBit ? GL_DEPTH_BUFFER_BIT : 0);
	glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, b, textureFilter[filter]);
}

Handle createShader(ShaderType type) {
	return glCreateShader(shaderType[type]);
}

void shaderSource(Handle shader, uint count, const char * const *src, const int *len) {
	glShaderSource(shader, count, src, len);
}

void compileShader(Handle shader) {
	glCompileShader(shader);
}

void getShaderiv(Handle shader, ShaderParam param, int *i) {
	glGetShaderiv(shader, shaderParam[param], i);
}

void getShaderInfoLog(Handle shader, uint max, int *len, char *log) {
	glGetShaderInfoLog(shader, max, len, log);
}

void deleteShader(Handle shader) {
	glDeleteShader(shader);
}

Handle createProgram() {
	return glCreateProgram();
}

void deleteProgram(Handle prog) {
	glDeleteProgram(prog);
}

void useProgram(Handle prog) {
	glUseProgram(prog);
}

void attachShader(Handle prog, Handle shader) {
	glAttachShader(prog, shader);
}

void linkProgram(Handle prog) {
	glLinkProgram(prog);
}

void getProgramiv(Handle prog, ShaderParam param, int *i) {
	glGetProgramiv(prog, shaderParam[param], i);
}

void getProgramInfoLog(Handle prog, uint max, int *len, char *log) {
	glGetProgramInfoLog(prog, max, len, log);
}
void getActiveUniform(Handle prog, uint index, uint max, int *len, int *size, UniformType *type, char *name) {
	glGetActiveUniform(prog, index, max, len, size, type, name);
}

void getActiveUniformBlockName(Handle prog, uint index, uint max, int *len, char *name) {
	glGetActiveUniformBlockName(prog, index, max, len, name);
}

int getUniformLocation(Handle shader, const char *name) {
	return glGetUniformLocation(shader, name);
}

uint getUniformBlockIndex(Handle shader, const char *name) {
	return glGetUniformBlockIndex(shader, name);
}
void uniformBlockBinding(Handle prog, uint index, uint binding) {
	glUniformBlockBinding(prog, index, binding);
}

void viewport(int srcX0, int srcY0, int srcX1, int srcY1) {
	glViewport(srcX0, srcY0, srcX1, srcY1);
}

void drawElementsInstancedBaseVertex(PrimitiveType mode, uint count, Type type, void *indices, uint primCount, uint baseVertex) {
	glDrawElementsInstancedBaseVertex(primitiveMode[mode], count, dataType[type], indices, primCount, baseVertex);
}

void enable(Feature fe) {
	glEnable(features[fe]);
}
void disable(Feature fe) {
	glDisable(features[fe]);
}

void depthFunc(DepthMode mode) {
	glDepthFunc(depthMode[mode]);
}

void blendFunc(BlendMode mode) {
	glBlendFunc(blendModeSrc[mode], blendModeDst[mode]);
}

void depthMask(bool mask) {
	glDepthMask(mask);
}

void cullFace(CullMode cull) {
	glCullFace(cullMode[cull]);
}

void colorMask(bool r, bool g, bool b, bool a) {
	glColorMask(r, g, b, a);
}

void texImage2D(TextureType target, int level, uint width, uint height, int border, TextureFormat format, const void *data) {
	glTexImage2D(textureType[target], level, format.internalFormat, width, height, border, format.format, format.type, data);
}

void generateMipmap(TextureType type) {
	glGenerateMipmap(textureType[type]);
}

uint64 getTextureSamplerHandle(Handle tex) {
	GLuint sampler = 0;
	glGenSamplers(1, &sampler);
	for(uint i = 0; i != 8; i++) {
		glBindSampler(i, sampler);
	}
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	return glGetTextureSamplerHandleARB(tex, sampler);
}
void makeTextureHandleResident(uint64 handle) {
	glMakeTextureHandleResidentARB(handle);
}




void programUniform1iv(Handle, int, uint, const int *) {}
void programUniform1uiv(Handle, int, uint, const uint *) {}
void programUniform1fv(Handle, int, uint, const float *) {}
void programUniform2iv(Handle, int, uint, const int *) {}
void programUniform3iv(Handle, int, uint, const int *) {}
void programUniform2fv(Handle, int, uint, const float *) {}
void programUniform3fv(Handle, int, uint, const float *) {}
void programUniform4fv(Handle, int, uint, const float *) {}
void programUniformMatrix2fv(Handle, int, uint, bool, const float *) {}
void programUniformMatrix3fv(Handle, int, uint, bool, const float *) {}
void programUniformMatrix4fv(Handle, int, uint, bool, const float *) {}
void programUniformHandleui64(Handle, int, uint64) {}

#endif




}
}
}
