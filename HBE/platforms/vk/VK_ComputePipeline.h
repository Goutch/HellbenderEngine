#pragma once

#include <core/resource/Texture.h>
#include <core/resource/RenderTarget.h>
#include "core/resource/ComputePipeline.h"

#include "vulkan/vulkan.h"

namespace HBE {
	class VK_Renderer;

	class VK_Fence;

	class VK_PipelineLayout;

	class VK_ComputePipeline : public ComputePipeline {
		VkPipeline handle = VK_NULL_HANDLE;
		VK_PipelineLayout *layout = nullptr;
		VK_Renderer *renderer = nullptr;
		VK_Fence *fence;
	public:
		~VK_ComputePipeline();

		void wait() override;
		void dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z) override;
		bool isDispatchFinished() override;
		void setTexture(uint32_t binding, const Texture *texture, uint32_t mip_level) override;
		void setTexture(uint32_t binding, const RenderTarget *render_target) override;
		void setTexture(const std::string &name, const Texture *texture, uint32_t mip_level) override;
		void setTexture(const std::string &name, const RenderTarget *render_target) override;
		VK_ComputePipeline(VK_Renderer *renderer, const ComputePipelineInfo &info);
	};
}


