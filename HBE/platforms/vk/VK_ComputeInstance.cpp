//
// Created by user on 8/24/2022.
//

#include "VK_ComputeInstance.h"
#include "VK_ComputePipeline.h"
#include "VK_Renderer.h"
#include "VK_PipelineDescriptors.h"
#include "VK_Fence.h"
#include "VK_Device.h"
#include "VK_CommandPool.h"

namespace HBE {

	VK_ComputeInstance::VK_ComputeInstance(VK_Renderer *renderer, const ComputeInstanceInfo &info) {
		const VK_ComputePipeline *compute_pipeline = dynamic_cast<const VK_ComputePipeline *>(info.compute_pipeline);
		const VK_PipelineLayout *layout = compute_pipeline->getPipelineLayout();
		descriptors = new VK_PipelineDescriptors(renderer, *layout);
		this->pipeline = compute_pipeline;
		fence = new VK_Fence(*renderer->getDevice());
	}

	VK_ComputeInstance::~VK_ComputeInstance() {
		wait();
		delete fence;
		delete descriptors;
	}

	const ComputePipeline *HBE::VK_ComputeInstance::getComputePipeline() const {
		return pipeline;
	}

	void VK_ComputeInstance::bind() const {
		descriptors->bind();
	}

	void VK_ComputeInstance::unbind() const {
		descriptors->unbind();
	}


	void VK_ComputeInstance::setDynamicUniform(uint32_t binding, const void *data) {
		descriptors->setDynamicUniform(binding, data);
	}


	void VK_ComputeInstance::setUniform(uint32_t binding, const void *data) {
		descriptors->setUniform(binding, data);
	}

	void VK_ComputeInstance::setTexture(uint32_t binding, const HBE::Texture *texture, uint32_t mip_level) {
		descriptors->setTexture(binding, texture, mip_level);
	}

	void VK_ComputeInstance::setTexture(uint32_t binding, const HBE::RenderTarget *render_target) {
		descriptors->setTexture(binding, render_target);
	}

	void VK_ComputeInstance::setDynamicUniform(const std::string &name, const void *data) {
		descriptors->setDynamicUniform(name, data);
	}

	void VK_ComputeInstance::setUniform(const std::string &name, const void *data) {
		descriptors->setUniform(name, data);
	}

	void VK_ComputeInstance::setTexture(const std::string &name, const Texture *texture, uint32_t mip_level) {
		descriptors->setTexture(name, texture, mip_level);
	}

	void VK_ComputeInstance::setTexture(const std::string &name, const RenderTarget *render_target) {
		descriptors->setTexture(name, render_target);
	}

	void VK_ComputeInstance::dispatch(uint32_t group_count_x,
									  uint32_t group_count_y,
									  uint32_t group_count_z) {
		fence->wait();

		VK_Queue &queue = renderer->getDevice()->getQueue(QUEUE_FAMILY_COMPUTE);
		queue.beginCommand();

		const VkCommandBuffer &command_buffer = queue.getCommandPool()->getCurrentBuffer();
		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->getHandle());

		descriptors->bind(command_buffer, renderer->getCurrentFrame());
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
		queue.endCommand();

		fence->reset();
		queue.submit(command_buffer, fence->getHandle());
	}

	bool VK_ComputeInstance::isDispatchFinished() {
		return fence->isSet();
	}

	void VK_ComputeInstance::wait() {
		fence->wait();
	}

}

