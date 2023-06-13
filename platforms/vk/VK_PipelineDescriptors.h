#pragma once


#include <string>
#include <core/resource/StorageBuffer.h>
#include "vulkan/vulkan.h"
#include "VK_Buffer.h"

#include "array"
#include "map"
#include "core/resource/TexelBuffer.h"

namespace HBE {
	class VK_PipelineLayout;

	class VK_Device;

	class VK_Renderer;

	class VK_GraphicPipeline;

	class VK_ComputePipeline;

	class VK_TopLevelAccelerationStructure;

	class GraphicPipelineInstance;

	class RenderTarget;

	class Texture;

	struct VariableDescriptor {
		uint32_t binding;
		uint32_t count;
		VkDescriptorType type;
	};
	struct DescriptorPool {
		VkDescriptorPool handle;
		std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> set_handles;
		std::array<std::vector<VkWriteDescriptorSet>, MAX_FRAMES_IN_FLIGHT> writes;
		std::unordered_map<uint32_t, VariableDescriptor> variable_descriptors;
	};


	class VK_PipelineDescriptors {
	protected:

		VK_Renderer *renderer = nullptr;
		VK_Device *device = nullptr;

		const VK_PipelineLayout *pipeline_layout = nullptr;

		DescriptorPool descriptor_pool;
		DescriptorPool temp_descriptor_pool;

		std::unordered_map<uint32_t, std::vector<VK_Buffer *>> uniform_buffers;

		std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings;
		std::map<std::string, size_t> uniform_name_to_index;
		std::map<uint32_t, size_t> uniform_binding_to_index;

		std::queue<std::pair<uint32_t, VkDescriptorPool>> old_descriptor_pools;

		mutable bool bound = false;
	public:
		VK_PipelineDescriptors(VK_Renderer *renderer, const VK_PipelineLayout &layout);
		~VK_PipelineDescriptors();

		void onFrameChange(uint32_t frame);
		void bind() const;
		uint32_t getBinding(const std::string &name) const;
		void bind(VkCommandBuffer command_buffer, uint32_t frame) const;
		void unbind() const;

		void setUniform(uint32_t binding, const void *data, int32_t frame);

		void setTexture(uint32_t binding, const Texture *texture, int32_t frame, uint32_t mip_level);
		void setTextureArray(uint32_t binding, Texture **textures, uint32_t texture_count, int32_t frame, int32_t mip_level);

		void setTexelBuffer(uint32_t binding, const TexelBuffer *buffer, int32_t frame, uint32_t mip_level);
		void setTexelBufferArray(uint32_t binding, TexelBuffer **buffers, uint32_t buffer_count, int32_t frame);

		void setAccelerationStructure(uint32_t binding, const VK_TopLevelAccelerationStructure *acceleration_structure, int32_t frame);

		void setStorageBuffer(uint32_t binding, StorageBuffer *buffer, int32_t frame);
		void setStorageBufferArray(uint32_t binding, StorageBuffer **buffers, uint32_t count, int32_t frame);

	private:
		void copyDescriptorSets(DescriptorPool &from, DescriptorPool &to, int32_t frame);
		void createDescriptorWrites(DescriptorPool &pool);
		void createDescriptorPool(DescriptorPool &pool);

		void createVariableSizeDescriptors(uint32_t binding, VkDescriptorType descriptor_type, uint32_t count, int32_t frame);
	};

}

