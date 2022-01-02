#pragma once

#include "core/resource/ComputePipeline.h"

#include "vulkan/vulkan.h"

namespace HBE {
	class VK_Renderer;
    class VK_PipelineLayout;
	class VK_ComputePipeline : public ComputePipeline {
		VkPipeline handle = VK_NULL_HANDLE;
		VK_PipelineLayout* layout = nullptr;
		VK_Renderer *renderer = nullptr;

		VK_ComputePipeline(VK_Renderer *renderer, ComputePipelineInfo &info);
        void dispatch(uint32_t group_size_x,uint32_t group_size_y,uint32_t group_size_z,uint32_t group_count_x,uint32_t group_count_y,uint32_t group_count_z);
        void wait() override;
	};
}


