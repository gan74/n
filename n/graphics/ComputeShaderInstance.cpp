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
#include "ComputeShaderInstance.h"

namespace n {
namespace graphics {

ComputeShaderInstance::ComputeShaderInstance(const Shader<ComputeShader> *ker) : ShaderInstanceBase(), kernel(ker) {
	const ShaderBase *b = static_cast<const ShaderBase *>(kernel);
	compile(&b, 1);
}

void ComputeShaderInstance::dispatch(const math::Vec3ui &size) {
	bind();
	validateState();
	gl::dispatchCompute(size);
}

const Shader<ComputeShader> *ComputeShaderInstance::getKernel() const {
	return kernel;
}

}
}
