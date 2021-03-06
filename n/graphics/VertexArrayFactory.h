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

#ifndef N_GRAPHICS_VERTEXARRAYFACTORY
#define N_GRAPHICS_VERTEXARRAYFACTORY

#include "DynamicBuffer.h"
#include "StaticBuffer.h"
#include "Vertex.h"
#include "VertexArrayObject.h"
#include <n/core/Timer.h>
#include <n/concurrent/LockGuard.h>

namespace n {
namespace graphics {

template<typename T>
class VertexArrayFactory : NonCopyable
{
	struct Empty
	{
		uint start;
		uint end;

		uint size() const {
			return end - start;
		}
	};


	public:
		static constexpr uint DefaultSize = 1024 * 1024;

		VertexArrayFactory() : vertices(DefaultSize), indexes(DefaultSize), handle(0) {
			setup();
			logMsg(core::String((vertices.size() * sizeof(Vertex<T>) + indexes.size() * sizeof(uint)) / 1024) + "kB of vertex data allocated");
		}

		VertexArrayObject<T> operator()(const typename TriangleBuffer<T>::FreezedTriangleBuffer &buff) {
			N_LOCK(mutex);

			mergeBlocks(vertexEmpties);
			mergeBlocks(indexEmpties);

			uint vert = alloc(vertexEmpties, buff.vertices.size());
			memcpy(vertices.begin() + vert, buff.vertices.begin(), buff.vertices.size() * sizeof(Vertex<T>));

			uint index = alloc(indexEmpties, buff.indexes.size());
			memcpy(indexes.begin() + index, buff.indexes.begin(), buff.indexes.size() * sizeof(uint));

			typename VertexArrayObject<T>::AllocData vao;
			vao.base = vert;
			vao.start = index;
			vao.object = this;
			vao.radius = buff.radius;
			vao.size = buff.indexes.size();
			vao.count = buff.vertices.size();

			return VertexArrayObject<T>(vao);
		}

	private:
		friend class VertexArrayObject<T>;
		friend class DrawCommandBuffer;

		TypedDynamicBuffer<Vertex<T>, ArrayBufferObject> vertices;
		TypedDynamicBuffer<uint, IndexBufferObject> indexes;
		StaticBuffer<uint, ArrayBufferObject> *ids;
		gl::Handle handle;

		core::Array<Empty> vertexEmpties;
		core::Array<Empty> indexEmpties;

		concurrent::Mutex mutex;

		uint alloc(core::Array<Empty> &empties, uint size) {
			if(empties.isEmpty()) {
				return fatal("Unable to allocate vertex data");
			}
			typename core::Array<Empty>::iterator best = empties.begin();
			for(typename core::Array<Empty>::iterator it = empties.begin(); it != empties.end(); it++) {
				if(it->size() >= size && it->size() > best->size()) {
					best = it;
				}
			}
			if(best->size() < size) {
				return fatal("Unable to allocate vertex data");
			}
			uint e = best->start;
			if(size == best->size()) {
				empties.erase(best);
			} else {
				best->start += size;
			}
			return e;
		}

		void mergeBlocks(core::Array<Empty> &empties) {
			if(empties.size() < 2) {
				return;
			}
			empties.sort([](Empty a, Empty b) {
				return a.start < b.start;
			});
			core::Array<Empty> merged;
			for(typename core::Array<Empty>::iterator it = empties.begin(); it < empties.end();) {
				typename core::Array<Empty>::iterator next = it + 1;
				while(next < empties.end()) {
					if(next->start != it->end) {
						break;
					}
					next++;
				}
				for(typename core::Array<Empty>::iterator i = it; i != next; i++) {
					merged.append(*i);
				}
				it = next;
			}
			empties = merged;
		}

		void setup() {
			core::Array<uint> idArray;
			for(uint i = 0; i != 2048; i++) {
				idArray.append(i);
			}
			ids = new StaticBuffer<uint, ArrayBufferObject>(idArray);

			vertexEmpties.append(Empty{0, vertices.size()});
			indexEmpties.append(Empty{0, indexes.size()});
			handle = gl::createVertexArray();
			gl::bindVertexArray(handle);
			vertices.update(true);
			indexes.update(true);
			gl::vertexAttribPointer(0, 3, GLType<T>::value, false, sizeof(Vertex<T>), 0);
			gl::vertexAttribPointer(1, 3, GLType<T>::value, false, sizeof(Vertex<T>), (void *)(2 * sizeof(T) + 3 * sizeof(T)));
			gl::vertexAttribPointer(2, 3, GLType<T>::value, false, sizeof(Vertex<T>), (void *)(2 * sizeof(T) + 2 * 3 * sizeof(T)));
			gl::vertexAttribPointer(3, 2, GLType<T>::value, false, sizeof(Vertex<T>), (void *)(3 * sizeof(T)));
			ids->bind();
			gl::vertexAttribPointer(4, 1, GLType<uint>::value, false, 0, 0, 1);
			gl::enableVertexAttribArray(0);
			gl::enableVertexAttribArray(1);
			gl::enableVertexAttribArray(2);
			gl::enableVertexAttribArray(3);
			gl::enableVertexAttribArray(4);
		}

		void bind() {
			setUnModified();
			gl::bindVertexArray(handle);
		}

		void setUnModified() {
			if(vertices.isModified()) {
				N_LOG_PERF;
				core::Timer timer;
				this->vertices.update();
				this->indexes.update();
				logMsg(core::String("Vertex buffer update : ") + round(timer.elapsed() * 10000) * 0.1 + "ms", PerfLog);
			}
		}

		void free(const typename VertexArrayObject<T>::AllocData &vao) {
			mutex.lock();
			vertexEmpties.append(Empty{vao.base, vao.base + vao.count});
			indexEmpties.append(Empty{vao.start, vao.start + vao.size});
			mutex.unlock();
		}
};


}
}

#endif // N_GRAPHICS_VERTEXARRAYFACTORY

