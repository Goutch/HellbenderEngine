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

	void VK_RaytracingPipelineInstance::setDynamicUniform(uint32_t binding, const void *data) {
		descriptors->setDynamicUniform(binding, data);
	}

	void VK_RaytracingPipelineInstance::setUniform(uint32_t binding, const void *data) {
		descriptors->setUniform(binding, data);
	}

	void VK_RaytracingPipelineInstance::setTexture(uint32_t binding, const Texture *texture, uint32_t mip_level) {
		descriptors->setTexture(binding, texture, mip_level);
	}

	void VK_RaytracingPipelineInstance::setTexture(uint32_t binding, const RenderTarget *render_target) {
		descriptors->setTexture(binding, render_target);
	}

	void VK_RaytracingPipelineInstance::setDynamicUniform(const std::string &name, const void *data) {
		descriptors->setDynamicUniform(name, data);
	}

	void VK_RaytracingPipelineInstance::setUniform(const std::string &name, const void *data) {
		descriptors->setUniform(name, data);
	}

	void VK_RaytracingPipelineInstance::setTexture(const std::string &name, const Texture *texture, uint32_t mip_level) {
		descriptors->setTexture(name, texture, mip_level);
	}

	void VK_RaytracingPipelineInstance::setTexture(const std::string &name, const RenderTarget *render_target) {
		descriptors->setTexture(name, render_target);
	}

	const VK_RaytracingPipeline *VK_RaytracingPipelineInstance::getPipeline() const {
		return pipeline;
	}

	VK_RaytracingPipelineInstance::~VK_RaytracingPipelineInstance() {
		delete descriptors;
	}
}