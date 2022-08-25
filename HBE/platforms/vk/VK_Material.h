#pragma once

#include "core/resource/Material.h"

namespace HBE {
	class VK_PipelineDescriptors;

	class GraphicPipeline;

	class VK_Renderer;

	class VK_Material : public Material {
		VK_PipelineDescriptors *descriptors;
		GraphicPipeline *pipeline;
	public:
		VK_Material(VK_Renderer *renderer, const MaterialInfo &info);
		~VK_Material() override;
		void bind() const override;
		void unbind() const override;
		void setDynamicUniform(const std::string &name, const void *data) override;
		void setDynamicUniform(uint32_t binding, const void *data) override;
		void setUniform(const std::string &name, const void *data) override;
		void setUniform(uint32_t binding, const void *data) override;
		void setTexture(uint32_t binding, const Texture *texture, uint32_t mip_level) override;
		void setTexture(uint32_t binding, const RenderTarget *render_target) override;
		void setTexture(const std::string &name, const Texture *texture, uint32_t mip_level) override;
		void setTexture(const std::string &name, const RenderTarget *render_target) override;
		const GraphicPipeline *getGraphicPipeline() const override;
	};
}
