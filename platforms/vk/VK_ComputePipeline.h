#pragma once

#include "core/resource/ComputePipeline.h"
#include "vulkan/vulkan.h"

namespace HBE
{
	class VK_Renderer;

	class VK_PipelineLayout;

	class VK_ComputePipeline : public ComputePipeline
	{
		VkPipeline handle = VK_NULL_HANDLE;
		VK_PipelineLayout* layout = nullptr;
		VK_Renderer* renderer = nullptr;
		ComputePipelineInfo info;

	public:
		const vec3i& getWorkgroupSize() const override;

	public:
		COMPUTE_PIPELINE_FLAGS getFlags() const override;
		VK_ComputePipeline(VK_Renderer* renderer, const ComputePipelineInfo& info);
		~VK_ComputePipeline();
		const VK_PipelineLayout* getPipelineLayout() const;
		VkPipeline getHandle() const;
	};
}
