#pragma once

#include "core/resource/raytracing/RaytracingPipeline.h"
#include "vulkan/vulkan.h"

namespace HBE {
	class VK_Renderer;

	class VK_Device;
	class VK_PipelineLayout;
	class VK_RaytracingPipeline : public RaytracingPipeline {
		VkPipeline handle;
		VK_Device *device = nullptr;
		VK_PipelineLayout *layout = nullptr;
		VK_RaytracingPipeline(VK_Renderer *renderer, const RaytracingPipelineInfo &info);
	};
}
