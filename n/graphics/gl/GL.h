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

#ifndef N_GRAPHICS_GL_GL
#define N_GRAPHICS_GL_GL

#include <n/defines.h>
#ifndef N_NO_GL

#include <n/types.h>
#include <GL/glew.h>
#include <GL/gl.h>

namespace n {
namespace graphics {
namespace gl {

template<typename T>
class GLType
{
	static constexpr GLenum value = 0;
};

template<>
class GLType<float>
{
	static_assert(sizeof(float) == 4, "float should be 4 byte long.");
	static constexpr GLenum value = GL_FLOAT;
};

template<>
class GLType<double>
{
	static_assert(sizeof(double) == 8, "double should be 8 byte long.");
	static constexpr GLenum value = GL_DOUBLE;
};

template<>
class GLType<int8>
{
	static constexpr GLenum value = GL_BYTE;
};

template<>
class GLType<uint8>
{
	static constexpr GLenum value = GL_UNSIGNED_BYTE;
};

template<>
class GLType<int16>
{
	static constexpr GLenum value = GL_SHORT;
};

template<>
class GLType<uint16>
{
	static constexpr GLenum value = GL_UNSIGNED_SHORT;
};

template<>
class GLType<int32>
{
	static constexpr GLenum value = GL_INT;
};

template<>
class GLType<uint32>
{
	static constexpr GLenum value = GL_UNSIGNED_INT;
};

enum BufferBinding
{
	Array = GL_ARRAY_BUFFER,
	Uniform = GL_UNIFORM_BUFFER,
	Index = GL_ELEMENT_ARRAY_BUFFER
};

GLenum getBufferUsage(BufferBinding b);

}
}
}

#endif


#endif // N_GRAPHICS_GL_GL
