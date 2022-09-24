
#include "VK_Material.h"
#include "VK_PipelineDescriptors.h"
#include "VK_GraphicPipeline.h"

namespace HBE {
	VK_Material::VK_Material(VK_Renderer *renderer, const MaterialInfo &info) {
		VK_GraphicPipeline *graphic_pipeline = dynamic_cast<VK_GraphicPipeline *>(info.graphic_pipeline);
		const VK_PipelineLayout *layout = graphic_pipeline->getPipelineLayout();
		descriptors = new VK_PipelineDescriptors(renderer, *layout);
		this->pipeline = info.graphic_pipeline;
		this->vk_pipeline = graphic_pipeline;
	}

	VK_Material::~VK_Material() {
		delete descriptors;
	}

	void VK_Material::bind() const {
		descriptors->bind();
	}

	void VK_Material::unbind() const {
		descriptors->unbind();
	}

	void VK_Material::setUniform(const std::string &name, const void *data, int32_t frame) {
		descriptors->setUniform(name, data, frame);
	}

	void VK_Material::setUniform(uint32_t binding, const void *data, int32_t frame) {
		descriptors->setUniform(binding, data, frame);
	}

	void VK_Material::setTexture(uint32_t binding, const Texture *texture, int32_t frame, uint32_t mip_level) {
		descriptors->setTexture(binding, texture, mip_level, frame);
	}

	void VK_Material::setTexture(const std::string &name, const Texture *texture, int32_t frame, uint32_t mip_level) {
		descriptors->setTexture(name, texture, frame, mip_level);
	}

	const GraphicPipeline *HBE::VK_Material::getGraphicPipeline() const {
		return pipeline;
	}

	VK_GraphicPipeline *VK_Material::getVkGraphicPipeline() {
		return vk_pipeline;
	}

	void VK_Material::setTextureArray(uint32_t binding, const Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) {
		descriptors->setTextureArray(binding, texture, texture_count, frame, mip_level);

	}

	void VK_Material::setTextureArray(const std::string &name, const Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) {
		descriptors->setTextureArray(name, texture, texture_count, frame, mip_level);
	}


}
