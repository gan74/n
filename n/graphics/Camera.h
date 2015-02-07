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

#ifndef N_GRAPHICS_CAMERA
#define N_GRAPHICS_CAMERA

#include "Transformable.h"
#include <n/math/Volume.h>
#include <n/utils.h>

namespace n {
namespace graphics {

class Camera final : public Transformable<>, public math::Volume<>
{
	protected:
		using Transformable<>::transform; // WHY ?
		using Transformable<>::radius; // WHY ?

	public:
		Camera() : zFar(1000), zNear(1), ratio(1), fov(math::pi<>() / 2) {
			radius = zFar;
			computeViewMatrix();
			computeProjectionMatrix();
			computeFrustum();
		}

		void setPosition(const math::Vec3 &pos) {
			transform = math::Transform<>(transform.getRotation(), pos);
			view[0][3] = -side.dot(pos);
			view[1][3] = -up.dot(pos);
			view[2][3] = -forward.dot(pos);
		}

		void setRotation(const math::Quaternion<> &q) {
			transform = math::Transform<>(q, transform.getPosition());
			computeViewMatrix();
			computeFrustum();
		}

		math::Vec3 getForward() const {
			return -forward;
		}

		void setFov(float f) {
			fov = f;
			computeProjectionMatrix();
			computeFrustum();
		}

		const math::Matrix4<> &getProjectionMatrix() const {
			return proj;
		}

		const math::Matrix4<> &getViewMatrix() const {
			return view;
		}

		virtual bool isInside(const math::Vec3 &p, float r) const override {
			math::Vec3 w = (p - transform.getPosition());
			float z = -w.dot(forward);
			return z + r > zNear && z - r < zFar
					&& w.dot(frustum[0]) + r > 0
					&& w.dot(frustum[1]) + r > 0
					&& w.dot(frustum[2]) + r > 0
					&& w.dot(frustum[3]) + r > 0;
		}

	private:
		void computeViewMatrix() {
			forward = -transform.getX();
			up = transform.getZ();
			side = -transform.getY();
			math::Vec3 p = transform.getPosition();
			view = math::Matrix4<>(side.x(), side.y(), side.z(), -side.dot(p),
			up.x(), up.y(), up.z(), -up.dot(p),
			forward.x(), forward.y(), forward.z(), -forward.dot(p),
			0, 0, 0, 1);
		}

		void computeProjectionMatrix() {
			float f = cos(fov / 2.0) / sin(fov / 2.0);
			float z = zFar - zNear;
			proj = math::Matrix4<>(1.0 / (tan(fov / 2) * ratio), 0, 0, 0, 0, f, 0, 0, 0, 0, -(zFar + zNear) / z, -1, 0, 0, -(2 * zFar * zNear) / z, 0).transposed();
		}

		void computeFrustum() {
			float fovR = fov / 2;
			float hFovR = atan(tan(fovR) * ratio);
			float c = cos(fovR);
			float s = sin(fovR);
			frustum[0] = -forward * s + up * c;
			frustum[1] = -forward * s - up * c;
			c = cos(hFovR);
			s = sin(hFovR);
			frustum[2] = -forward * s + side * c;
			frustum[3] = -forward * s - side * c;
		}


		float zFar;
		float zNear;

		float ratio;
		float fov;

		math::Vec3 forward;
		math::Vec3 up;
		math::Vec3 side;

		math::Matrix4<> view;
		math::Matrix4<> proj;


		math::Vec3 frustum[4];
};


}
}

#endif // N_GRAPHICS_CAMERA
