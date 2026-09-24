#pragma once
#include "Core.h"

namespace HBE {
	struct HB_API GraphicLimits {
		uint32_t max_3D_texture_size;
		uint32_t max_2D_texture_size;
		uint32_t max_1D_texture_size;
		uint32_t max_storage_buffer_size;
	};
}
