
#include "VK_ComputePipeline.h"
#include "VK_Renderer.h"
#include "VK_Device.h"
#include "VK_Shader.h"

namespace HBE {
	VK_ComputePipeline::VK_ComputePipeline(VK_Renderer *renderer, ComputePipelineInfo &info) {
		this->renderer = renderer;
		VK_Shader *vk_shader = dynamic_cast<VK_Shader *>(info.compute_shader);


		VkPipelineLayoutCreateInfo layout_create_info{};
		layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		//todo:layout_create_info.pSetLayouts=

		if(vkCreatePipelineLayout(renderer->getDevice()->getHandle(),&layout_create_info, nullptr,&layout)!=VK_SUCCESS)
		{
			Log::error("Failed to create compute pipeline layout");
		}

		VkPipelineShaderStageCreateInfo stage{};
		stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		stage.module = vk_shader->getHandle();

		VkComputePipelineCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		create_info.layout = layout;
		create_info.stage = stage;


		if (vkCreateComputePipelines(renderer->getDevice()->getHandle(), VK_NULL_HANDLE, 1, &create_info, nullptr, &handle) != VK_SUCCESS) {
			Log::error("Failed to create compute pipeline");
		}


	}
}

