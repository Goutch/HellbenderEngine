#include "VK_RasterizationPipelineInstance.h"
#include "VK_PipelineDescriptors.h"
#include "VK_RasterizationPipeline.h"

namespace HBE
{
	VK_RasterizationPipelineInstance::VK_RasterizationPipelineInstance(VK_Renderer* renderer, const RasterizationPipelineInstanceInfo& info)
	{
		VK_RasterizationPipeline* graphic_pipeline = dynamic_cast<VK_RasterizationPipeline*>(info.rasterization_pipeline);
		const VK_PipelineLayout* layout = graphic_pipeline->getPipelineLayout();
		this->pipeline = info.rasterization_pipeline;
		this->vk_pipeline = graphic_pipeline;
		descriptors = new VK_PipelineDescriptors(renderer,
		                                         *layout,
		                                         info.uniform_memory_type_infos,
		                                         info.uniform_memory_type_info_count,
		                                         pipeline->getFlags() & RASTERIZATION_PIPELINE_FLAG_ALLOW_EMPTY_DESCRIPTOR);
	}

	VK_RasterizationPipelineInstance::~VK_RasterizationPipelineInstance()
	{
		delete descriptors;
	}

	const RasterizationPipeline* HBE::VK_RasterizationPipelineInstance::getGraphicPipeline() const
	{
		return pipeline;
	}

	VK_RasterizationPipeline* VK_RasterizationPipelineInstance::getVkGraphicPipeline()
	{
		return vk_pipeline;
	}

	void VK_RasterizationPipelineInstance::bind() const
	{
		descriptors->bind();
	}

	void VK_RasterizationPipelineInstance::unbind() const
	{
		descriptors->unbind();
	}

	void VK_RasterizationPipelineInstance::setUniform(const std::string& name, const void* data, int32_t frame)
	{
		descriptors->setUniform(descriptors->getBinding(name), data, frame);
	}

	void VK_RasterizationPipelineInstance::setUniform(uint32_t binding, const void* data, int32_t frame)
	{
		descriptors->setUniform(binding, data, frame);
	}

	void VK_RasterizationPipelineInstance::setImage(uint32_t binding, const Image* texture, int32_t frame, uint32_t mip_level)
	{
		descriptors->setTexture(binding, texture, mip_level, frame);
	}

	void VK_RasterizationPipelineInstance::setImage(const std::string& name, const Image* texture, int32_t frame, uint32_t mip_level)
	{
		descriptors->setTexture(descriptors->getBinding(name), texture, frame, mip_level);
	}


	void VK_RasterizationPipelineInstance::setImageArray(uint32_t binding, Image** texture, uint32_t texture_count, int32_t frame, uint32_t mip_level)
	{
		descriptors->setTextureArray(binding, texture, texture_count, frame, mip_level);
	}

	void VK_RasterizationPipelineInstance::setImageArray(const std::string& name, Image** texture, uint32_t texture_count, int32_t frame, uint32_t mip_level)
	{
		descriptors->setTextureArray(descriptors->getBinding(name), texture, texture_count, frame, mip_level);
	}

	void VK_RasterizationPipelineInstance::setStorageBuffer(uint32_t binding, StorageBuffer* buffer, size_t count, size_t offset, int32_t frame)
	{
		descriptors->setStorageBuffer(binding, buffer, count, offset, frame);
	}

	void VK_RasterizationPipelineInstance::setStorageBufferArray(uint32_t binding, StorageBuffer** buffers, uint32_t count, int32_t frame)
	{
		descriptors->setStorageBufferArray(binding, buffers, count, frame);
	}

	void VK_RasterizationPipelineInstance::setStorageBuffer(const std::string& name, StorageBuffer* buffer, size_t count, size_t offset, int32_t frame)
	{
		descriptors->setStorageBuffer(descriptors->getBinding(name), buffer, 0, 0, frame);
	}

	void VK_RasterizationPipelineInstance::setStorageBufferArray(const std::string& name, StorageBuffer** buffers, uint32_t count, int32_t frame)
	{
		descriptors->setStorageBufferArray(descriptors->getBinding(name), buffers, count, frame);
	}

	void VK_RasterizationPipelineInstance::setTexelBuffer(const std::string& name, TexelBuffer* buffer, int32_t frame)
	{
		descriptors->setTexelBuffer(descriptors->getBinding(name), buffer, frame);
	}

	void VK_RasterizationPipelineInstance::setTexelBufferArray(const std::string& name, TexelBuffer** buffers, uint32_t count, int32_t frame)
	{
		descriptors->setTexelBufferArray(descriptors->getBinding(name), buffers, count, frame);
	}

	void VK_RasterizationPipelineInstance::setTexelBuffer(uint32_t binding, TexelBuffer* buffer, int32_t frame)
	{
		descriptors->setTexelBuffer(binding, buffer, frame);
	}

	void VK_RasterizationPipelineInstance::setTexelBufferArray(uint32_t binding, TexelBuffer** buffers, uint32_t count, int32_t frame)
	{
		descriptors->setTexelBufferArray(binding, buffers, count, frame);
	}
}
