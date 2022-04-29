
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
		fence = new VK_Fence(*renderer->getDevice());
		layout = new VK_PipelineLayout(renderer->getDevice(), &vk_shader, 1);

		VkPipelineShaderStageCreateInfo stage{};
		stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		stage.module = vk_shader->getHandle();
		stage.pName="main";
		VkComputePipelineCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		create_info.layout = layout->getHandle();
		create_info.stage = stage;

		if (vkCreateComputePipelines(renderer->getDevice()->getHandle(), VK_NULL_HANDLE, 1, &create_info, nullptr,
									 &handle) != VK_SUCCESS) {
			Log::error("Failed to create compute pipeline");
		}
	}

	void VK_ComputePipeline::dispatch(uint32_t group_count_x,
									  uint32_t group_count_y,
									  uint32_t group_count_z) {
		fence->wait();

		VK_Queue *queue = renderer->getDevice()->getQueue(QUEUE_FAMILY_COMPUTE);
		queue->beginCommand();

		const VkCommandBuffer &command_buffer = queue->getCommandPool()->getCurrentBuffer();
		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, handle);

		layout->bindDescriptors(command_buffer, renderer->getCurrentFrame());
		vkCmdDispatch(command_buffer, group_count_x, group_count_y, group_count_z);

		/*vkCmdPipelineBarrier(command_buffer,
							 VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
							 VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
							 0,
							 0,
							 nullptr,
							 0,
							 nullptr,
							 0,
							 nullptr);
		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, handle);*/
		queue->endCommand();

		fence->reset();
		queue->submit(command_buffer, fence->getHandle());
	}

	bool VK_ComputePipeline::isDispatchFinished() {
		return fence->isSet();
	}

	VK_ComputePipeline::~VK_ComputePipeline() {
		vkDestroyPipeline(renderer->getDevice()->getHandle(), handle, nullptr);
		delete layout;
		delete fence;
	}

	void VK_ComputePipeline::wait() {
		fence->wait();
	}

	void VK_ComputePipeline::setTexture(uint32_t binding, const Texture *texture, uint32_t mip_level) {
		layout->setTexture(binding, texture, mip_level);
	}

	void VK_ComputePipeline::setTexture(uint32_t binding, const RenderTarget *render_target) {
		layout->setTexture(binding, render_target);
	}

	void VK_ComputePipeline::setTexture(const std::string &name, const Texture *texture, uint32_t mip_level) {
		layout->setTexture(name, texture, mip_level);
	}

	void VK_ComputePipeline::setTexture(const std::string &name, const RenderTarget *render_target) {
		layout->setTexture(name, render_target);
	}


}

