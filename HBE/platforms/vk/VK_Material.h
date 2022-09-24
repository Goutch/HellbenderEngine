#pragma once

#include "core/resource/Material.h"

namespace HBE {
	class VK_PipelineDescriptors;

	class GraphicPipeline;

	class VK_Renderer;

	class VK_GraphicPipeline;

	class VK_Material : public Material {
		VK_PipelineDescriptors *descriptors;
	public:
		void setTextureArray(const std::string &name, const Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) override;
	private:
		GraphicPipeline *pipeline;
	public:
		void setTextureArray(uint32_t binding, const Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) override;
	private:
		VK_GraphicPipeline *vk_pipeline;
	public:
		VK_Material(VK_Renderer *renderer, const MaterialInfo &info);
		~VK_Material() override;
		void bind() const override;
		void unbind() const override;
		void setUniform(const std::string &name, const void *data, int32_t frame) override;
		void setUniform(uint32_t binding, const void *data, int32_t frame) override;
		void setTexture(uint32_t binding, const Texture *texture, int32_t frame, uint32_t mip_level) override;
		void setTexture(const std::string &name, const Texture *texture, int32_t frame, uint32_t mip_level) override;
		const GraphicPipeline *getGraphicPipeline() const override;
		VK_GraphicPipeline *getVkGraphicPipeline();

	};
}
