#pragma once

#include <core/resource/ComputeInstance.h>
#include <string>

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
		void setTextureArray(uint32_t binding, const Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) override;
	private:
		VK_Renderer *renderer;
		VK_Fence *fence;
	public:
		VK_ComputeInstance(VK_Renderer *renderer, const ComputeInstanceInfo &info);
		~VK_ComputeInstance() override;
		void bind() const override;
		void unbind() const override;

		void setUniform(uint32_t binding, const void *data, int32_t frame) override;
		void setTexture(uint32_t binding, const Texture *texture, int32_t frame, uint32_t mip_level) override;
		void setUniform(const std::string &name, const void *data, int32_t frame) override;
		void setTexture(const std::string &name, const Texture *texture, int32_t frame, uint32_t mip_level) override;
		void dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z) override;
		bool isDispatchFinished() override;
		void wait() override;
		void setTextureArray(const std::string &name, const Texture **texture, uint32_t texture_count, int32_t frame, uint32_t mip_level) override;

		void setStorageBuffer(uint32_t binding, StorageBuffer *buffer, int32_t frame) override;
		void setStorageBufferArray(uint32_t binding, StorageBuffer **buffer, uint32_t count, int32_t frame) override;

		void setStorageBuffer(const std::string &name, StorageBuffer *buffer, int32_t frame) override;
		void setStorageBufferArray(const std::string &name, StorageBuffer **buffer, uint32_t count, int32_t frame) override;
		const ComputePipeline *getComputePipeline() const override;
	};
}

