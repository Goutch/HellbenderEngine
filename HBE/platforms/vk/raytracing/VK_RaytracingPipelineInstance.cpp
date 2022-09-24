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
		const VK_TopLevelAccelerationStructure *top_level_acceleration_structure = dynamic_cast<const VK_TopLevelAccelerationStructure *>(info.root_acceleration_structure);
		descriptors->setAccelerationStructure("topLevelAS", top_level_acceleration_structure);
		pipeline = raytracing_pipeline;
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
		descriptors->setUniform(name, data, frame);
	}

	void VK_RaytracingPipelineInstance::setTexture(const std::string &name, const Texture *texture, int32_t frame, uint32_t mip_level) {
		descriptors->setTexture(name, texture, frame, mip_level);
	}

	const VK_RaytracingPipeline *VK_RaytracingPipelineInstance::getPipeline() const {
		return pipeline;
	}

	VK_RaytracingPipelineInstance::~VK_RaytracingPipelineInstance() {
		delete descriptors;
	}

	void VK_RaytracingPipelineInstance::setTextureArray(const std::string &name, const Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) {
		descriptors->setTextureArray(name, texture, texture_count, frame, mip_level);
	}

	void VK_RaytracingPipelineInstance::setTextureArray(uint32_t binding, const Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) {
		descriptors->setTextureArray(binding, texture, texture_count, mip_level, frame);

	}
}
