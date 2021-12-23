#pragma once

#include "core/resource/ComputePipeline.h"

#include "vulkan/vulkan.h"

namespace HBE {
	class VK_Renderer;

	class VK_ComputePipeline : public ComputePipeline {
		VkPipeline handle = VK_NULL_HANDLE;
		VkPipelineLayout layout = VK_NULL_HANDLE;
		VK_Renderer *renderer = nullptr;

		VK_ComputePipeline(VK_Renderer *renderer, ComputePipelineInfo &info);

	};
}


