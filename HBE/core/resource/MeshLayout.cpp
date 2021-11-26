#include "MeshLayout.h"

namespace HBE {

	uint32_t MeshLayout::getBytesPerVertex() const {
		return getBytes(vertex_layout);
	}

	uint32_t MeshLayout::getBytesPerInstance() const {
		return getBytes(instanced_layout);
	}

	uint32_t MeshLayout::getBytes(const std::vector<GLSL_TYPE> &types) const {
		uint32_t bytes = 0;
		for (size_t i = 0; i < types.size(); ++i) {
			switch (types[i]) {
				case GLSL_TYPE::FLOAT:
				case GLSL_TYPE::INT:
				case GLSL_TYPE::UINT:
					bytes += 4;
					break;
				case GLSL_TYPE::VEC2F:
				case GLSL_TYPE::VEC2I:
				case GLSL_TYPE::VEC2UI:
					bytes += 8;
					break;
				case GLSL_TYPE::VEC3F:
				case GLSL_TYPE::VEC3I:
				case GLSL_TYPE::VEC3UI:
					bytes += 12;
					break;
				case GLSL_TYPE::VEC4F:
				case GLSL_TYPE::VEC4I:
				case GLSL_TYPE::VEC4UI:
					bytes += 16;
				case GLSL_TYPE::MAT3F:
				case GLSL_TYPE::MAT3I:
				case GLSL_TYPE::MAT3UI:
					bytes += 36;
					break;
				case GLSL_TYPE::MAT4F:
				case GLSL_TYPE::MAT4I:
				case GLSL_TYPE::MAT4UI:
					bytes += 64;
					break;
			}
		}
		return bytes;
	}


}