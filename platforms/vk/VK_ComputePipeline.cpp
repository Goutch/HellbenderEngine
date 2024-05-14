
#include "VK_ComputePipeline.h"
#include "VK_Renderer.h"
#include "VK_Device.h"
#include "VK_Shader.h"
#include "VK_PipelineLayout.h"
#include "VK_PipelineLayout.h"
#include "VK_CommandPool.h"
#include "VK_Fence.h"

namespace HBE {
	VK_ComputePipeline::VK_ComputePipeline(VK_Renderer *renderer, const ComputePipelineInfo &info) {
		this->renderer = renderer;
		const VK_Shader *vk_shader = dynamic_cast<VK_Shader *>(info.compute_shader);
		layout = new VK_PipelineLayout(renderer->getDevice(), &vk_shader, 1, info.flags & COMPUTE_PIPELINE_FLAG_ALLOW_EMPTY_DESCRIPTOR);

		VkPipelineShaderStageCreateInfo stage{};
		stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		stage.module = vk_shader->getHandle();
		stage.pName = "main";
		VkComputePipelineCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		create_info.layout = layout->getHandle();
		create_info.stage = stage;

		if (vkCreateComputePipelines(renderer->getDevice()->getHandle(), VK_NULL_HANDLE, 1, &create_info, nullptr,
		                             &handle) != VK_SUCCESS) {
			Log::error("Failed to create compute pipeline");
		}
	}


	VK_ComputePipeline::~VK_ComputePipeline() {
		vkDestroyPipeline(renderer->getDevice()->getHandle(), handle, nullptr);
		delete layout;

	}


	const VK_PipelineLayout *VK_ComputePipeline::getPipelineLayout() const {
		return layout;
	}

	VkPipeline VK_ComputePipeline::getHandle() const {
		return handle;
	}


}

