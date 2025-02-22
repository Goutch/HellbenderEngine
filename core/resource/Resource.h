#pragma once

#include "Core.h"
#include "HBETypes.h"

namespace HBE {


	typedef uint32_t VERTEX_ATTRIBUTE_FLAGS;
	enum VERTEX_ATTRIBUTE_FLAG {
		VERTEX_ATTRIBUTE_FLAG_NONE = 0,
		VERTEX_ATTRIBUTE_FLAG_PER_INSTANCE = 1 << 0,/// Disable device local memory and skip staging buffer. May result in slower read in shader.
		VERTEX_ATTRIBUTE_FLAG_FAST_WRITE = 1 << 1,///Recommended for buffer changing every frame, dont wait for rendering to finish but double/triple memory usage.
		VERTEX_ATTRIBUTE_FLAG_MULTIPLE_BUFFERS = 1 << 2,
	};
	enum VERTEX_TOPOLOGY {
		VERTEX_TOPOLOGY_TRIANGLE,
		VERTEX_TOPOLOGY_LINE,
	};

	struct VertexAttributeInfo {
		uint32_t location = 0;
		uint32_t size = 0;
		VERTEX_ATTRIBUTE_FLAGS flags = VERTEX_ATTRIBUTE_FLAG_NONE;
	};

	const VertexAttributeInfo VERTEX_ATTRIBUTE_INFO_POSITION3D = {0, sizeof(vec3), VERTEX_ATTRIBUTE_FLAG_NONE};
	const VertexAttributeInfo VERTEX_ATTRIBUTE_INFO_POSITION3D_NORMAL_INTERLEAVED = {0, sizeof(vec3) + sizeof(vec3), VERTEX_ATTRIBUTE_FLAG_NONE};
	const VertexAttributeInfo VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED = {0, sizeof(vec3) + sizeof(vec2), VERTEX_ATTRIBUTE_FLAG_NONE};
	const VertexAttributeInfo VERTEX_ATTRIBUTE_INFO_POSITION3D_NORMAL_UV_INTERLEAVED = {0, sizeof(vec3) + sizeof(vec3) + sizeof(vec2), VERTEX_ATTRIBUTE_FLAG_NONE};

	const VertexAttributeInfo VERTEX_ATTRIBUTE_INFO_POSITION2D = {0, sizeof(vec2), VERTEX_ATTRIBUTE_FLAG_NONE};
	const VertexAttributeInfo VERTEX_ATTRIBUTE_INFO_POSITION2D_NORMAL_INTERLEAVED = {0, sizeof(vec2) + sizeof(vec3), VERTEX_ATTRIBUTE_FLAG_NONE};
	const VertexAttributeInfo VERTEX_ATTRIBUTE_INFO_POSITION2D_UV_INTERLEAVED = {0, sizeof(vec2) + sizeof(vec2), VERTEX_ATTRIBUTE_FLAG_NONE};
	const VertexAttributeInfo VERTEX_ATTRIBUTE_INFO_POSITION2D_NORMAL_UV_INTERLEAVED = {0, sizeof(vec2) + sizeof(vec3) + sizeof(vec2), VERTEX_ATTRIBUTE_FLAG_NONE};

	class Resource {
	protected:
		Resource &operator=(const Resource &) = delete;

		Resource(const Resource &) = delete;

		Resource() = default;

	public:

		virtual ~Resource() = default;
	};

}
