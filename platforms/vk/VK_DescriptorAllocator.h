#pragma once

#include "vulkan/vulkan.h"
#include "vector"
namespace HBE {
	class VK_DescriptorAllocator {
		std::vector<VkDescriptorPool> pools;
	};
}
