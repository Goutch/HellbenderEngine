
#pragma once

#include <cstdint>

namespace HBE {
	typedef uint64_t entity_handle;
	struct EntityRef {
		size_t index;
		entity_handle handle;
	};
}
