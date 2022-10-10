#pragma once

#include "core/resource/GraphicPipelineInstance.h"

namespace HBE {
	class VK_PipelineDescriptors;

	class GraphicPipeline;

	class VK_Renderer;

	class VK_GraphicPipeline;

	class StorageBuffer;

	class VK_GraphicPipelineInstance : public GraphicPipelineInstance {
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
		VK_GraphicPipelineInstance(VK_Renderer *renderer, const MaterialInfo &info);
		~VK_GraphicPipelineInstance() override;
		void bind() const override;
		void unbind() const override;
		void setUniform(const std::string &name, const void *data, int32_t frame) override;
		void setUniform(uint32_t binding, const void *data, int32_t frame) override;
		void setTexture(uint32_t binding, const Texture *texture, int32_t frame, uint32_t mip_level) override;
		void setTexture(const std::string &name, const Texture *texture, int32_t frame, uint32_t mip_level) override;
		void setStorageBuffer(uint32_t binding, StorageBuffer *buffer, int32_t frame) override;
		void setStorageBufferArray(uint32_t binding, StorageBuffer **buffer, uint32_t count, int32_t frame) override;
		void setStorageBuffer(const std::string &name, StorageBuffer *buffer, int32_t frame) override;
		void setStorageBufferArray(const std::string &name, StorageBuffer **buffers, uint32_t count, int32_t frame) override;
		const GraphicPipeline *getGraphicPipeline() const override;
		VK_GraphicPipeline *getVkGraphicPipeline();

	};
}
