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

#ifndef N_GRAPHICS_VERTEXBUFFER
#define N_GRAPHICS_VERTEXBUFFER

#include "Vertex.h"

namespace n {
namespace graphics {

template<typename T = float>
class VertexBuffer
{
	public:
		VertexBuffer(const core::Array<Vertex<T>> &a = core::Array<Vertex<T>>()) : vertices(a) {
		}

		uint append(const Vertex<T> &v) {
			vertices.append(v);
			return vertices.size() - 1;
		}

		const Vertex<T> &operator()(uint v) const {
			return vertices[v];
		}

		Vertex<T> &operator[](uint v) {
			return vertices[v];
		}

		const Vertex<T> &operator[](uint v) const {
			return vertices[v];
		}

		typename core::Array<Vertex<T>>::iterator begin() {
			return vertices.begin();
		}

		typename core::Array<Vertex<T>>::iterator end() {
			return vertices.end();
		}

		typename core::Array<Vertex<T>>::const_iterator begin() const {
			return vertices.begin();
		}

		typename core::Array<Vertex<T>>::const_iterator end() const {
			return vertices.end();
		}

		uint size() const {
			return vertices.size();
		}

		const core::Array<Vertex<T>> &getVertices() const {
			return vertices;
		}

	private:
		core::Array<Vertex<T>> vertices;
};

}
}

#endif // N_GRAPHICS_VERTEXBUFFER

