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

namespace HBE
{
	VK_ComputeInstance::VK_ComputeInstance(VK_Renderer* renderer, const ComputeInstanceInfo& info)
	{
		const VK_ComputePipeline* compute_pipeline = dynamic_cast<const VK_ComputePipeline*>(info.compute_pipeline);
		const VK_PipelineLayout* layout = compute_pipeline->getPipelineLayout();
		this->pipeline = compute_pipeline;
		descriptors = new VK_PipelineDescriptors(renderer,
		                                         *layout,
		                                         info.preferred_memory_type_flags,
		                                         info.uniform_memory_type_infos,
		                                         info.uniform_memory_type_info_count,
		                                         info.
		                                         compute_pipeline->getFlags() & COMPUTE_PIPELINE_FLAG_ALLOW_EMPTY_DESCRIPTOR);;

		fence = new VK_Fence(*renderer->getDevice());
		this->renderer = renderer;
	}

	VK_ComputeInstance::~VK_ComputeInstance()
	{
		wait();
		delete fence;
		delete descriptors;
	}

	const ComputePipeline* HBE::VK_ComputeInstance::getComputePipeline() const
	{
		return pipeline;
	}

	void VK_ComputeInstance::bind() const
	{
		descriptors->bind();
	}

	void VK_ComputeInstance::unbind() const
	{
		descriptors->unbind();
	}

	void VK_ComputeInstance::setUniform(uint32_t binding, const void* data, int32_t frame)
	{
		descriptors->setUniform(binding, data, frame);
	}

	void VK_ComputeInstance::setImage(uint32_t binding, const Image* texture, int32_t frame, uint32_t mip_level)
	{
		descriptors->setTexture(binding, texture, mip_level, frame);
	}

	void VK_ComputeInstance::setUniform(const std::string& name, const void* data, int32_t frame)
	{
		descriptors->setUniform(descriptors->getBinding(name), data, frame);
	}

	void VK_ComputeInstance::setImage(const std::string& name, const Image* texture, int32_t frame, uint32_t mip_level)
	{
		descriptors->setTexture(descriptors->getBinding(name), texture, frame, mip_level);
	}

	void VK_ComputeInstance::dispatchAsync(uint32_t size_x,
	                                       uint32_t size_y,
	                                       uint32_t size_z)
	{
		fence->wait();
		VK_Device* device = renderer->getDevice();

		VK_Queue& queue = device->hasQueue(QUEUE_FAMILY_COMPUTE) ? device->getQueue(QUEUE_FAMILY_COMPUTE) : device->getQueue(QUEUE_FAMILY_GRAPHICS);
		queue.beginCommand();

		const VkCommandBuffer& command_buffer = queue.getCommandPool()->getCurrentBuffer();
		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->getHandle());

		descriptors->bind(command_buffer, renderer->getCurrentFrame());
		vkCmdDispatch(command_buffer,
		              static_cast<uint32_t>(ceil(pipeline->getWorkgroupSize().x / float(size_x))),
		              static_cast<uint32_t>(ceil(pipeline->getWorkgroupSize().y / float(size_y))),
		              static_cast<uint32_t>(ceil(pipeline->getWorkgroupSize().z / float(size_x))));

		queue.endCommand();

		fence->reset();
		queue.submit(command_buffer, fence->getHandle());
	}

	bool VK_ComputeInstance::isDispatchFinished()
	{
		return fence->isSet();
	}

	void VK_ComputeInstance::wait()
	{
		fence->wait();
	}

	void VK_ComputeInstance::setImageArray(uint32_t binding, Image** texture, uint32_t texture_count, int32_t frame, uint32_t mip_level)
	{
		descriptors->setTextureArray(binding, texture, texture_count, frame, mip_level);
	}

	void VK_ComputeInstance::setImageArray(const std::string& name, Image** texture, uint32_t texture_count, int32_t frame, uint32_t mip_level)
	{
		descriptors->setTextureArray(descriptors->getBinding(name), texture, texture_count, frame, mip_level);
	}

	void VK_ComputeInstance::setStorageBuffer(uint32_t binding, StorageBuffer* buffer, size_t count, size_t offset, int32_t frame)
	{
		descriptors->setStorageBuffer(binding, buffer, count, offset, frame);
	}

	void VK_ComputeInstance::setStorageBufferArray(uint32_t binding, StorageBuffer** buffers, uint32_t count, int32_t frame)
	{
		descriptors->setStorageBufferArray(binding, buffers, count, frame);
	}

	void VK_ComputeInstance::setStorageBuffer(const std::string& name, StorageBuffer* buffer, size_t count, size_t offset, int32_t frame)
	{
		descriptors->setStorageBuffer(descriptors->getBinding(name), buffer, 0, 0, frame);
	}

	void VK_ComputeInstance::setStorageBufferArray(const std::string& name, StorageBuffer** buffer, uint32_t count, int32_t frame)
	{
		descriptors->setStorageBufferArray(descriptors->getBinding(name), buffer, count, frame);
	}

	void VK_ComputeInstance::setTexelBuffer(uint32_t binding, TexelBuffer* buffer, int32_t frame)
	{
		descriptors->setTexelBuffer(binding, buffer, frame);
	}

	void VK_ComputeInstance::setTexelBufferArray(uint32_t binding, TexelBuffer** buffers, uint32_t count, int32_t frame)
	{
		descriptors->setTexelBufferArray(binding, buffers, count, frame);
	}

	void VK_ComputeInstance::setTexelBuffer(const std::string& name, TexelBuffer* buffer, int32_t frame)
	{
		descriptors->setTexelBuffer(descriptors->getBinding(name), buffer, frame);
	}

	void VK_ComputeInstance::setTexelBufferArray(const std::string& name, TexelBuffer** buffers, uint32_t count, int32_t frame)
	{
		descriptors->setTexelBufferArray(descriptors->getBinding(name), buffers, count, frame);
	}
}
