#pragma once

#include "core/resource/RasterizationPipelineInstance.h"

namespace HBE {
	class VK_PipelineDescriptors;

	class RaterizationPipeline;

	class VK_Renderer;

	class VK_GraphicPipeline;

	class StorageBuffer;

	class VK_GraphicPipelineInstance : public RasterizationPipelineInstance {
		VK_PipelineDescriptors *descriptors;
		RaterizationPipeline *pipeline;
		VK_GraphicPipeline *vk_pipeline;

		bool bound = false;
	public:
		VK_GraphicPipelineInstance(VK_Renderer *renderer, const GraphicPipelineInstanceInfo &info);
		~VK_GraphicPipelineInstance() override;

		//---------------------------IPipelineInstance Implementation---------------------------
		void bind() const override;
		void unbind() const override;

		void setUniform(const std::string &name, const void *data, int32_t frame) override;
		void setUniform(uint32_t binding, const void *data, int32_t frame) override;

		void setTexture(uint32_t binding, const Texture *texture, int32_t frame, uint32_t mip_level) override;
		void setTexture(const std::string &name, const Texture *texture, int32_t frame, uint32_t mip_level) override;
		void setTextureArray(const std::string &name, Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) override;
		void setTextureArray(uint32_t binding, Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) override;

		void setStorageBuffer(uint32_t binding, StorageBuffer *buffer, size_t count, size_t offset, int32_t frame) override;
		void setStorageBufferArray(uint32_t binding, StorageBuffer **buffer, uint32_t count, int32_t frame) override;
		void setStorageBuffer(const std::string &name, StorageBuffer *buffer, size_t count, size_t offset, int32_t frame) override;
		void setStorageBufferArray(const std::string &name, StorageBuffer **buffers, uint32_t count, int32_t frame) override;

		void setTexelBuffer(const std::string &name, TexelBuffer *buffer, int32_t frame) override;
		void setTexelBufferArray(const std::string &name, TexelBuffer **buffers, uint32_t count, int32_t frame) override;
		void setTexelBuffer(uint32_t binding, TexelBuffer *buffer, int32_t frame) override;
		void setTexelBufferArray(uint32_t binding, TexelBuffer **buffers, uint32_t count, int32_t frame) override;

		const RaterizationPipeline *getGraphicPipeline() const override;
		VK_GraphicPipeline *getVkGraphicPipeline();

	};
}
