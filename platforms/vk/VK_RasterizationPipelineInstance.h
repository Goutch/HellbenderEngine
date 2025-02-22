#pragma once

#include "core/resource/RasterizationPipelineInstance.h"

namespace HBE {
	class VK_PipelineDescriptors;

	class RasterizationPipeline;

	class VK_Renderer;

	class VK_RasterizationPipeline;

	class StorageBuffer;

	class VK_RasterizationPipelineInstance : public RasterizationPipelineInstance {
		VK_PipelineDescriptors *descriptors;
		RasterizationPipeline *pipeline;
		VK_RasterizationPipeline *vk_pipeline;

		bool bound = false;
	public:
		VK_RasterizationPipelineInstance(VK_Renderer *renderer, const RasterizationPipelineInstanceInfo &info);
		~VK_RasterizationPipelineInstance() override;

		//---------------------------IPipelineInstance Implementation---------------------------
		void bind() const override;
		void unbind() const override;

		void setUniform(const std::string &name, const void *data, int32_t frame) override;
		void setUniform(uint32_t binding, const void *data, int32_t frame) override;

		void setImage(uint32_t binding, const Image *texture, int32_t frame, uint32_t mip_level) override;
		void setImage(const std::string &name, const Image *texture, int32_t frame, uint32_t mip_level) override;
		void setImageArray(const std::string &name, Image **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) override;
		void setImageArray(uint32_t binding, Image **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) override;

		void setStorageBuffer(uint32_t binding, StorageBuffer *buffer, size_t count, size_t offset, int32_t frame) override;
		void setStorageBufferArray(uint32_t binding, StorageBuffer **buffer, uint32_t count, int32_t frame) override;
		void setStorageBuffer(const std::string &name, StorageBuffer *buffer, size_t count, size_t offset, int32_t frame) override;
		void setStorageBufferArray(const std::string &name, StorageBuffer **buffers, uint32_t count, int32_t frame) override;

		void setTexelBuffer(const std::string &name, TexelBuffer *buffer, int32_t frame) override;
		void setTexelBufferArray(const std::string &name, TexelBuffer **buffers, uint32_t count, int32_t frame) override;
		void setTexelBuffer(uint32_t binding, TexelBuffer *buffer, int32_t frame) override;
		void setTexelBufferArray(uint32_t binding, TexelBuffer **buffers, uint32_t count, int32_t frame) override;

		const RasterizationPipeline *getGraphicPipeline() const override;
		VK_RasterizationPipeline *getVkGraphicPipeline();

	};
}
