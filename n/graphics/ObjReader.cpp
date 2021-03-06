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


#include <iostream>
#include <n/io/File.h>
#include "MeshLoader.h"
#include "MaterialLoader.h"
#include <n/core/Timer.h>

namespace n {
namespace graphics {

class ObjReader : public MeshLoader::AssetReader<ObjReader, core::String>
{
	public:
		ObjReader() : MeshLoader::AssetReader<ObjReader, core::String>() {
		}

		core::Array<SubMeshInstance> *operator()(core::String name) override {
			if(!name.endsWith(".obj")) {
				return 0;
			}
			io::File file(name);
			return load(file);
		}

	private:
		core::Array<SubMeshInstance> *load(io::File &file) {
			if(!file.open(io::IODevice::Read)) {
				std::cerr<<file.getName()<<" not found"<<std::endl;
				return 0;
			}
			N_LOG_PERF;
			core::String path = file.getPath();
			//core::Timer timer;
			uint fs = file.size();
			char *data = new char[fs + 1];
			data[file.readBytes(data)] = 0;
			core::Array<core::String> lines = core::String(data).split("\n");
			delete[] data;
			file.close();
			core::Array<math::Vec3> positions;
			positions.append(math::Vec3());
			core::Array<math::Vec3> normals;
			normals.append(math::Vec3());
			core::Array<math::Vec2> coords;
			coords.append(math::Vec2());

			for(const core::String &l : lines) {
				if(l.size() < 4) {
					continue;
				}
				if(l.beginsWith("v ")) {
					/*core::Array<float> fl = l.subString(2).split(" ");//.mapped([](const core::String &s) { return s.to<float>(); });
					if(fl.size() != 3) {
						std::cerr<<"Invalid position"<<std::endl;
						return 0;
					}*/
					float fl[3] = {0};
					sscanf(l.toChar(), "v %f %f %f", &fl[0], &fl[1], &fl[2]);
					positions.append(math::Vec3(fl[0], fl[1], fl[2]));
				} else if(l.beginsWith("vn ")) {
					/*core::Array<float> fl = l.subString(3).split(" ");//.mapped([](const core::String &s) { return s.to<float>(); });
					if(fl.size() != 3) {
						std::cerr<<"Invalid normal"<<std::endl;
						return 0;
					}*/
					float fl[3] = {0};
					sscanf(l.toChar(), "vn %f %f %f", &fl[0], &fl[1], &fl[2]);
					normals.append(math::Vec3(fl[0], fl[1], fl[2]));
				} else if(l.beginsWith("vt ")) {
					/*core::Array<float> fl = l.subString(3).split(" ");//.mapped([](const core::String &s) { return s.to<float>(); });
					if(fl.size() != 2) {
						std::cerr<<"Invalid texture coord"<<std::endl;
						return 0;
					}*/
					float fl[2] = {0};
					sscanf(l.toChar(), "vt %f %f", &fl[0], &fl[1]);
					coords.append(math::Vec2(fl[0], fl[1]));
				}
			}
			core::String mtllib;
			core::Map<math::Vec3ui, uint> vmap;
			bool smooth = false;
			TriangleBuffer<> tr;
			Material mat;
			core::Array<SubMeshInstance> *bases = new core::Array<SubMeshInstance>;
			for(const core::String &l : lines) {
				if(l.beginsWith("f ")) {
					core::Array<core::String> fl = l.subString(2).split(" ");
					uint face[] = {0, 0, 0, 0};
					uint vertCount = fl.size();
					if(vertCount < 3 || vertCount > 4) {
						std::cerr<<"Invalid (non triangle/non quad) face. \""<<l<<"\""<<std::endl;
						return 0;
					}
					bool noNorm = false;
					for(uint i = 0; i != vertCount; i++) {
						math::Vec3ui v;
						sscanf(fl[i].toChar(), "%u/%u/%u", &v[0], &v[1], &v[2]);
						if(smooth) {
							v[2] = 0;
						}
						noNorm |= !v[2];
						core::Map<math::Vec3ui, uint>::const_iterator it = vmap.find(v);
						if(it == vmap.end()) {
							if(v.x() >= positions.size() || v.z() >= normals.size() || v.y() >= coords.size()) {
								std::cerr<<"Index out of bound : "<<core::String(v)<<std::endl;
								return 0;
							}
							Vertex<> vert = Vertex<>(positions[v.x()], normals[v.z()], coords[v.y()]);
							uint index = tr.append(vert);
							vmap.insert(v, index);
							face[i] = index;
						} else {
							face[i] = (*it)._2;
						}
					}
					if(noNorm && !smooth) {
						math::Vec3 n = tr.getNormal(face[0], face[1], face[2]);
						for(uint i = 0; i != vertCount; i++) {
							Vertex<> v = tr.getVertices()[face[i]];
							v.n() = n;
							face[i] = tr.append(v);
						}
					}
					tr.append(face[0], face[1], face[2]);
					if(vertCount == 4) {
						tr.append(face[2], face[3], face[0]);
					}
				} else if(l.beginsWith("s ")) {
					core::String sm = l.subString(2).toLower().filtered([](char c) { return !isspace(c); });
					smooth = !(sm == "off" || sm == "0");
				} else if(l.beginsWith("usemtl ")) {
					if(!tr.getTriangles().isEmpty()) {
						bases->append(SubMeshInstance(std::move(tr.freezed()), mat));
						tr = TriangleBuffer<>();
						vmap.clear();
					}
					mat = MaterialLoader::load<core::String, core::String>(mtllib, l.subString(7));
				} else if(l.beginsWith("mtllib ")) {
					mtllib = l.subString(7);
					if(!io::File::exists(mtllib)) {
						mtllib = path + "/" + mtllib;
					}
				}
			}
			if(!tr.getTriangles().isEmpty()) {
				bases->append(SubMeshInstance(std::move(tr.freezed()), mat));
			}
			return bases;
		}
};

}
}

