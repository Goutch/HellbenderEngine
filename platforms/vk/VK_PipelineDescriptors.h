#pragma once


#include <string>
#include <core/resource/StorageBuffer.h>
#include "vulkan/vulkan.h"
#include "VK_Buffer.h"

#include "array"
#include "map"
#include "core/resource/IPipelineInstance.h"
#include "core/resource/TexelBuffer.h"

namespace HBE
{
	class VK_PipelineLayout;

	class VK_Device;

	class VK_Renderer;

	class VK_RasterizationPipeline;

	class VK_ComputePipeline;

	class VK_TopLevelAccelerationStructure;

	class RasterizationPipelineInstance;

	class RenderTarget;

	class Image;

	struct VariableDescriptorSet
	{
		uint32_t binding;
		uint32_t count = 1;
		VkDescriptorType type;
	};

	struct DescriptorPool
	{
		VkDescriptorPool handle;
		std::vector<VkDescriptorSet> descriptor_set_handles; //frame0 set0 |frame0 set1 | frame1 set0 | frame1 set1| frame2 set0 | frame2 set1
		std::array<std::vector<VkWriteDescriptorSet>, MAX_FRAMES_IN_FLIGHT> writes;
		std::vector<VariableDescriptorSet> variable_descriptor_sets; //frame0 set0 |frame0 set1 | frame1 set0 | frame1 set1| frame2 set0 | frame2 set1
	};

	class VK_PipelineDescriptors
	{
	protected:
		bool empty_descriptor_allowed;
		VK_Renderer* renderer = nullptr;
		VK_Device* device = nullptr;

		const VK_PipelineLayout* pipeline_layout = nullptr;

		DescriptorPool descriptor_pool;
		DescriptorPool temp_descriptor_pool;

		std::array<std::vector<VK_Buffer*>, MAX_FRAMES_IN_FLIGHT> uniform_buffers;

		std::queue<std::pair<uint32_t, VkDescriptorPool>> old_descriptor_pools;

		mutable bool bound = false;

	public:

		VK_PipelineDescriptors(VK_Renderer* renderer,
		                       const VK_PipelineLayout& layout,
		                       MEMORY_TYPE_FLAGS preferred_memory_type_flags,
		                       UniformMemoryInfo* uniform_memory_type_infos,
		                       uint32_t uniform_memory_type_info_count,
		                       bool empty_descriptor_allowed = false);
		~VK_PipelineDescriptors();

		void onFrameChange(uint32_t frame);

		void bind() const;

		uint32_t getBinding(const std::string& name) const;

		void resetPool(DescriptorPool& pool);

		void bind(VkCommandBuffer command_buffer, uint32_t frame) const;

		void unbind() const;

		void setUniform(uint32_t binding, const void* data, int32_t frame);

		void setTexture(uint32_t binding, const Image* texture, int32_t frame, uint32_t mip_level);

		void setTextureArray(uint32_t binding, Image** textures, uint32_t texture_count, int32_t frame, int32_t mip_level);

		void setTexelBuffer(uint32_t binding, const TexelBuffer* buffer, int32_t frame);

		void setTexelBufferArray(uint32_t binding, TexelBuffer** buffers, uint32_t buffer_count, int32_t frame);

		void setAccelerationStructure(uint32_t binding, const VK_TopLevelAccelerationStructure* acceleration_structure, int32_t frame);

		void setStorageBuffer(uint32_t binding, StorageBuffer* buffer, size_t count, size_t offset, int32_t frame);

		void setStorageBufferArray(uint32_t binding, StorageBuffer** buffers, uint32_t count, int32_t frame);

	private:
		void copyDescriptorSets(DescriptorPool& from, DescriptorPool& to, int32_t frame);

		void createDescriptorWrites(DescriptorPool& pool);

		void createDescriptorPool(DescriptorPool& pool);

		void createVariableSizeDescriptors(uint32_t binding, VkDescriptorType descriptor_type, uint32_t count, int32_t frame);
	};
}
