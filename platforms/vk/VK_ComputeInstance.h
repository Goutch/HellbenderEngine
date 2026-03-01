#pragma once

#include <core/resource/ComputeInstance.h>
#include <string>

#include "VK_Fence.h"

namespace HBE {
	class VK_Renderer;

	class VK_PipelineDescriptors;

	class ComputePipeline;

	class VK_Fence;

	class VK_ComputePipeline;

	class VK_ComputeInstance : public ComputeInstance {
		const VK_ComputePipeline *pipeline;
		VK_PipelineDescriptors *descriptors;
	public:

	private:
		VK_Renderer *renderer;
		VK_Fence fence;
	public:
		VK_ComputeInstance(VK_Renderer *renderer, const ComputeInstanceInfo &info);
		~VK_ComputeInstance() override;


		void dispatchAsync(uint32_t size_x, uint32_t size_y, uint32_t size_z) override;
		bool isDispatchFinished() override;
		void wait() override;

		//---------------------------IPipelineInstance Implementation---------------------------
		void bind() const override;
		void unbind() const override;

		void setUniform(uint32_t binding, const void *data, int32_t frame) override;
		void setUniform(const std::string &name, const void *data, int32_t frame) override;

		void setImage(uint32_t binding, const Image *texture, int32_t frame, uint32_t mip_level) override;
		void setImageArray(uint32_t binding, Image **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) override;
		void setImage(const std::string &name, const Image *texture, int32_t frame, uint32_t mip_level) override;
		void setImageArray(const std::string &name, Image **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) override;

		void setStorageBuffer(uint32_t binding, StorageBuffer *buffer, size_t count, size_t offset, int32_t frame) override;
		void setStorageBufferArray(uint32_t binding, StorageBuffer **buffer, uint32_t count, int32_t frame) override;
		void setStorageBuffer(const std::string &name, StorageBuffer *buffer, size_t count, size_t offset, int32_t frame) override;
		void setStorageBufferArray(const std::string &name, StorageBuffer **buffer, uint32_t count, int32_t frame) override;

		void setTexelBuffer(uint32_t binding, TexelBuffer *buffer, int32_t frame) override;
		void setTexelBufferArray(uint32_t binding, TexelBuffer **buffers, uint32_t count, int32_t frame) override;
		void setTexelBuffer(const std::string &name, TexelBuffer *buffer, int32_t frame) override;
		void setTexelBufferArray(const std::string &name, TexelBuffer **buffers, uint32_t count, int32_t frame) override;

		const ComputePipeline *getComputePipeline() const override;
	};
}

