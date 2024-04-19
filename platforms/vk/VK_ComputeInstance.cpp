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


	void VK_ComputeInstance::setUniform(uint32_t binding, const void *data, int32_t frame) {
		descriptors->setUniform(binding, data, frame);
	}

	void VK_ComputeInstance::setTexture(uint32_t binding, const Texture *texture, int32_t frame, uint32_t mip_level) {
		descriptors->setTexture(binding, texture, mip_level, frame);
	}

	void VK_ComputeInstance::setUniform(const std::string &name, const void *data, int32_t frame) {
		descriptors->setUniform(descriptors->getBinding(name), data, frame);
	}

	void VK_ComputeInstance::setTexture(const std::string &name, const Texture *texture, int32_t frame, uint32_t mip_level) {
		descriptors->setTexture(descriptors->getBinding(name), texture, frame, mip_level);
	}

	void VK_ComputeInstance::dispatch(uint32_t group_count_x,
									  uint32_t group_count_y,
									  uint32_t group_count_z) {
		fence->wait();
		VK_Device *device = renderer->getDevice();

		VK_Queue &queue = device->hasQueue(QUEUE_FAMILY_TRANSFER) ? device->getQueue(QUEUE_FAMILY_COMPUTE) : device->getQueue(QUEUE_FAMILY_GRAPHICS);
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

	void VK_ComputeInstance::setTextureArray(uint32_t binding, Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) {
		descriptors->setTextureArray(binding, texture, texture_count, frame, mip_level);
	}

	void VK_ComputeInstance::setTextureArray(const std::string &name, Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) {
		descriptors->setTextureArray(descriptors->getBinding(name), texture, texture_count, frame, mip_level);
	}

	void VK_ComputeInstance::setStorageBuffer(uint32_t binding, StorageBuffer *buffer, size_t count, size_t offset, int32_t frame) {
		descriptors->setStorageBuffer(binding, buffer, count, offset, frame);
	}

	void VK_ComputeInstance::setStorageBufferArray(uint32_t binding, StorageBuffer **buffers, uint32_t count, int32_t frame) {
		descriptors->setStorageBufferArray(binding, buffers, count, frame);
	}

	void VK_ComputeInstance::setStorageBuffer(const std::string &name, StorageBuffer *buffer, size_t count, size_t offset, int32_t frame) {
		descriptors->setStorageBuffer(descriptors->getBinding(name), buffer, 0, 0, frame);
	}

	void VK_ComputeInstance::setStorageBufferArray(const std::string &name, StorageBuffer **buffer, uint32_t count, int32_t frame) {
		descriptors->setStorageBufferArray(descriptors->getBinding(name), buffer, count, frame);
	}

	void VK_ComputeInstance::setTexelBuffer(uint32_t binding, TexelBuffer *buffer, int32_t frame) {
		descriptors->setTexelBuffer(binding, buffer, frame);
	}

	void VK_ComputeInstance::setTexelBufferArray(uint32_t binding, TexelBuffer **buffers, uint32_t count, int32_t frame) {
		descriptors->setTexelBufferArray(binding, buffers, count, frame);
	}

	void VK_ComputeInstance::setTexelBuffer(const std::string &name, TexelBuffer *buffer, int32_t frame) {
		descriptors->setTexelBuffer(descriptors->getBinding(name), buffer, frame);
	}

	void VK_ComputeInstance::setTexelBufferArray(const std::string &name, TexelBuffer **buffers, uint32_t count, int32_t frame) {
		descriptors->setTexelBufferArray(descriptors->getBinding(name), buffers, count, frame);
	}

}

