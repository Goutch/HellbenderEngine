#pragma once

#include "core/resource/raytracing/RaytracingPipelineInstance.h"

namespace HBE {
	class VK_Renderer;

	class VK_RaytracingPipeline;

	class VK_PipelineLayout;

	class VK_PipelineDescriptors;

	class VK_RaytracingPipelineInstance : public RaytracingPipelineInstance {
		VK_RaytracingPipeline *pipeline;
		VK_PipelineDescriptors *descriptors = nullptr;
	public:
		VK_RaytracingPipelineInstance(VK_Renderer &renderer, const RaytracingPipelineInstanceInfo &info);
		~VK_RaytracingPipelineInstance() override;
		void bind() const override;
		void unbind() const override;
		void setDynamicUniform(uint32_t binding, const void *data) override;
		void setUniform(uint32_t binding, const void *data) override;
		void setTexture(uint32_t binding, const Texture *texture, uint32_t mip_level) override;
		void setTexture(uint32_t binding, const RenderTarget *render_target) override;
		void setDynamicUniform(const std::string &name, const void *data) override;
		void setUniform(const std::string &name, const void *data) override;
		void setTexture(const std::string &name, const Texture *texture, uint32_t mip_level) override;
		void setTexture(const std::string &name, const RenderTarget *render_target) override;
		const VK_RaytracingPipeline *getPipeline() const;
	};

}
