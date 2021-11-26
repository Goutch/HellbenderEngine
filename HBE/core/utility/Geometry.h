#pragma once

#include "Core.h"
#include "core/resource/Mesh.h"

namespace HBE {
	typedef uint32_t VERTEX_FLAGS;
	enum VERTEX_FLAG {
		VERTEX_FLAG_NONE = 0,
		VERTEX_FLAG_UV = 1,
		VERTEX_FLAG_NORMAL = 2,
	};
	struct HB_API UsedBuffers {
		bool vertex_position = true;
		bool vertex_uv = true;
		bool vertex_normal = true;
	};

	class HB_API Geometry {


	public:
		static void createQuad(Mesh &mesh, float size_x, float size_y, VERTEX_FLAGS flags);

		static void createCube(Mesh &mesh, float size_x, float size_y, float size_z, VERTEX_FLAGS flags);
	};
}