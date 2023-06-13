//
// Created by user on 8/30/2022.
//

#include "VK_RaytracingPipelineInstance.h"
#include "VK_RaytracingPipeline.h"
#include "../VK_Renderer.h"
#include "../VK_PipelineLayout.h"
#include "../VK_PipelineDescriptors.h"
#include "VK_TopLevelAccelerationStructure.h"

namespace HBE {
	VK_RaytracingPipelineInstance::VK_RaytracingPipelineInstance(VK_Renderer &renderer, const RaytracingPipelineInstanceInfo &info) {
		VK_RaytracingPipeline *raytracing_pipeline = dynamic_cast<VK_RaytracingPipeline *>(info.raytracing_pipeline);
		const VK_PipelineLayout *layout = raytracing_pipeline->getPipelineLayout();
		descriptors = new VK_PipelineDescriptors(&renderer, *layout);
		pipeline = raytracing_pipeline;
	}

	VK_RaytracingPipelineInstance::~VK_RaytracingPipelineInstance() {
		delete descriptors;
	}

	const VK_RaytracingPipeline *VK_RaytracingPipelineInstance::getPipeline() const {
		return pipeline;
	}

	void VK_RaytracingPipelineInstance::bind() const {
		descriptors->bind();
	}

	void VK_RaytracingPipelineInstance::unbind() const {
		descriptors->unbind();
	}

	void VK_RaytracingPipelineInstance::setUniform(uint32_t binding, const void *data, int32_t frame) {
		descriptors->setUniform(binding, data, frame);
	}

	void VK_RaytracingPipelineInstance::setTexture(uint32_t binding, const Texture *texture, int32_t frame, uint32_t mip_level) {
		descriptors->setTexture(binding, texture, mip_level, frame);
	}

	void VK_RaytracingPipelineInstance::setUniform(const std::string &name, const void *data, int32_t frame) {
		descriptors->setUniform(descriptors->getBinding(name), data, frame);
	}

	void VK_RaytracingPipelineInstance::setTexture(const std::string &name, const Texture *texture, int32_t frame, uint32_t mip_level) {
		descriptors->setTexture(descriptors->getBinding(name), texture, frame, mip_level);
	}

	void VK_RaytracingPipelineInstance::setTextureArray(const std::string &name, Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) {
		descriptors->setTextureArray(descriptors->getBinding(name), texture, texture_count, frame, mip_level);
	}

	void VK_RaytracingPipelineInstance::setTextureArray(uint32_t binding, Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) {
		descriptors->setTextureArray(binding, texture, texture_count, mip_level, frame);
	}

	void VK_RaytracingPipelineInstance::setStorageBuffer(uint32_t binding, StorageBuffer *buffer, int32_t frame) {
		descriptors->setStorageBuffer(binding, buffer, frame);
	}

	void VK_RaytracingPipelineInstance::setStorageBufferArray(uint32_t binding, StorageBuffer **buffer, uint32_t count, int32_t frame) {
		descriptors->setStorageBufferArray(binding, buffer, count, frame);
	}

	void VK_RaytracingPipelineInstance::setStorageBufferArray(const std::string &name, StorageBuffer **buffer, uint32_t count, int32_t frame) {
		descriptors->setStorageBufferArray(descriptors->getBinding(name), buffer, count, frame);
	}

	void VK_RaytracingPipelineInstance::setStorageBuffer(const std::string &name, StorageBuffer *buffer, int32_t frame) {
		descriptors->setStorageBuffer(descriptors->getBinding(name), buffer, frame);
	}

	void VK_RaytracingPipelineInstance::setAccelerationStructure(uint32_t binding, const RootAccelerationStructure *accelerationStructure, int32_t frame) {
		descriptors->setAccelerationStructure(binding, dynamic_cast<const VK_TopLevelAccelerationStructure *>(accelerationStructure), frame);
	}

	void VK_RaytracingPipelineInstance::setAccelerationStructure(const std::string &name, const RootAccelerationStructure *accelerationStructure, int32_t frame) {
		descriptors->setAccelerationStructure(descriptors->getBinding(name), dynamic_cast<const VK_TopLevelAccelerationStructure *>(accelerationStructure), frame);
	}

	void VK_RaytracingPipelineInstance::setTexelBuffer(uint32_t binding, TexelBuffer *buffer, int32_t frame) {
		descriptors->setTexelBuffer(binding, buffer, frame);
	}

	void VK_RaytracingPipelineInstance::setTexelBufferArray(uint32_t binding, TexelBuffer **buffers, uint32_t count, int32_t frame) {
		descriptors->setTexelBufferArray(binding, buffers, count, frame);
	}

	void VK_RaytracingPipelineInstance::setTexelBuffer(const std::string &name, TexelBuffer *buffer, int32_t frame) {
		descriptors->setTexelBuffer(descriptors->getBinding(name), buffer, frame);
	}

	void VK_RaytracingPipelineInstance::setTexelBufferArray(const std::string &name, TexelBuffer **buffers, uint32_t count, int32_t frame) {
		descriptors->setTexelBufferArray(descriptors->getBinding(name), buffers, count, frame);
	}
}
