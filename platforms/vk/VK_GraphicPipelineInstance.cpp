
#include "VK_GraphicPipelineInstance.h"
#include "VK_PipelineDescriptors.h"
#include "VK_GraphicPipeline.h"

namespace HBE {
	VK_GraphicPipelineInstance::VK_GraphicPipelineInstance(VK_Renderer *renderer, const RasterizationPipelineInstanceInfo &info) {
		VK_GraphicPipeline *graphic_pipeline = dynamic_cast<VK_GraphicPipeline *>(info.rasterization_pipeline);
		const VK_PipelineLayout *layout = graphic_pipeline->getPipelineLayout();
		descriptors = new VK_PipelineDescriptors(renderer, *layout);
		this->pipeline = info.rasterization_pipeline;
		this->vk_pipeline = graphic_pipeline;
	}

	VK_GraphicPipelineInstance::~VK_GraphicPipelineInstance() {
		delete descriptors;
	}

	const RasterizationPipeline *HBE::VK_GraphicPipelineInstance::getGraphicPipeline() const {
		return pipeline;
	}

	VK_GraphicPipeline *VK_GraphicPipelineInstance::getVkGraphicPipeline() {
		return vk_pipeline;
	}

	void VK_GraphicPipelineInstance::bind() const {
		descriptors->bind();
	}

	void VK_GraphicPipelineInstance::unbind() const {
		descriptors->unbind();
	}

	void VK_GraphicPipelineInstance::setUniform(const std::string &name, const void *data, int32_t frame) {
		descriptors->setUniform(descriptors->getBinding(name), data, frame);
	}

	void VK_GraphicPipelineInstance::setUniform(uint32_t binding, const void *data, int32_t frame) {
		descriptors->setUniform(binding, data, frame);
	}

	void VK_GraphicPipelineInstance::setTexture(uint32_t binding, const Texture *texture, int32_t frame, uint32_t mip_level) {
		descriptors->setTexture(binding, texture, mip_level, frame);
	}

	void VK_GraphicPipelineInstance::setTexture(const std::string &name, const Texture *texture, int32_t frame, uint32_t mip_level) {
		descriptors->setTexture(descriptors->getBinding(name), texture, frame, mip_level);
	}


	void VK_GraphicPipelineInstance::setTextureArray(uint32_t binding, Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) {
		descriptors->setTextureArray(binding, texture, texture_count, frame, mip_level);
	}

	void VK_GraphicPipelineInstance::setTextureArray(const std::string &name, Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) {
		descriptors->setTextureArray(descriptors->getBinding(name), texture, texture_count, frame, mip_level);
	}

	void VK_GraphicPipelineInstance::setStorageBuffer(uint32_t binding, StorageBuffer *buffer, size_t count, size_t offset, int32_t frame) {
		descriptors->setStorageBuffer(binding, buffer, count, offset, frame);
	}

	void VK_GraphicPipelineInstance::setStorageBufferArray(uint32_t binding, StorageBuffer **buffers, uint32_t count, int32_t frame) {
		descriptors->setStorageBufferArray(binding, buffers, count, frame);
	}

	void VK_GraphicPipelineInstance::setStorageBuffer(const std::string &name, StorageBuffer *buffer, size_t count, size_t offset, int32_t frame) {
		descriptors->setStorageBuffer(descriptors->getBinding(name), buffer, 0, 0, frame);
	}

	void VK_GraphicPipelineInstance::setStorageBufferArray(const std::string &name, StorageBuffer **buffers, uint32_t count, int32_t frame) {
		descriptors->setStorageBufferArray(descriptors->getBinding(name), buffers, count, frame);
	}

	void VK_GraphicPipelineInstance::setTexelBuffer(const std::string &name, TexelBuffer *buffer, int32_t frame) {
		descriptors->setTexelBuffer(descriptors->getBinding(name), buffer, frame);
	}

	void VK_GraphicPipelineInstance::setTexelBufferArray(const std::string &name, TexelBuffer **buffers, uint32_t count, int32_t frame) {
		descriptors->setTexelBufferArray(descriptors->getBinding(name), buffers, count, frame);
	}

	void VK_GraphicPipelineInstance::setTexelBuffer(uint32_t binding, TexelBuffer *buffer, int32_t frame) {
		descriptors->setTexelBuffer(binding, buffer, frame);
	}

	void VK_GraphicPipelineInstance::setTexelBufferArray(uint32_t binding, TexelBuffer **buffers, uint32_t count, int32_t frame) {
		descriptors->setTexelBufferArray(binding, buffers, count, frame);
	}


}
