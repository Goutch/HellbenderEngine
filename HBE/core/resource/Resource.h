#pragma once
#include "Core.h"
namespace HBE {

	typedef uint32_t VERTEX_BINDING_FLAGS;
	enum VERTEX_BINDING_FLAG {
		VERTEX_BINDING_FLAG_NONE = 0,
		VERTEX_BINDING_FLAG_PER_INSTANCE = 1 << 0,
		/// Disable device local memory and skip staging buffer. May result in slower read in shader.
		VERTEX_BINDING_FLAG_FAST_WRITE = 1 << 1,
		///Recommended for buffer changing every frame, dont wait for rendering to finish but double/triple memory usage.
		VERTEX_BINDING_FLAG_MULTIPLE_BUFFERS = 1 << 2,
	};
	enum VERTEX_TOPOLOGY {
		VERTEX_TOPOLOGY_TRIANGLE,
		VERTEX_TOPOLOGY_LINE,
	};
	struct VertexBindingInfo {
		uint32_t binding = 0;
		uint32_t size = 0;
		VERTEX_BINDING_FLAGS flags = VERTEX_BINDING_FLAG_NONE;
	};

	class Resource {
	protected:
		Resource &operator=(const Resource &) = delete;

		Resource(const Resource &) = delete;

		Resource() = default;

	public:
		virtual ~Resource() = default;
	};

}
