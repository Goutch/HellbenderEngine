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
		void setUniform(uint32_t binding, const void *data, int32_t frame) override;
		void setTexture(uint32_t binding, const Texture *texture, int32_t frame, uint32_t mip_level) override;
		void setUniform(const std::string &name, const void *data, int32_t frame) override;
		void setTexture(const std::string &name, const Texture *texture, int32_t frame, uint32_t mip_level) override;
		void setTextureArray(uint32_t binding, const Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) override;
		const VK_RaytracingPipeline *getPipeline() const;
		void setTextureArray(const std::string &name, const Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) override;
	};

}
