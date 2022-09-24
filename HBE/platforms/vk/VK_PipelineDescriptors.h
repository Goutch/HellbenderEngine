#pragma once


#include <string>
#include "vulkan/vulkan.h"
#include "VK_CONSTANTS.h"
#include "VK_Buffer.h"

#include "array"
#include "map"

namespace HBE {
	class VK_PipelineLayout;

	class VK_Device;

	class VK_Renderer;

	class VK_GraphicPipeline;

	class VK_ComputePipeline;

	class VK_TopLevelAccelerationStructure;

	class Material;

	class RenderTarget;

	class Texture;

	class VK_PipelineDescriptors {
	protected:

		VK_Renderer *renderer = nullptr;
		VK_Device *device = nullptr;

		const VK_PipelineLayout *pipeline_layout = nullptr;

		VkDescriptorPool pool_handle = VK_NULL_HANDLE;
		std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> handles;

		std::unordered_map<uint32_t, std::vector<VK_Buffer *>> uniform_buffers;
		std::array<std::unordered_map<uint32_t, VkWriteDescriptorSet>, MAX_FRAMES_IN_FLIGHT> descriptor_sets_writes;

		std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings;
		std::map<std::string, size_t> uniform_name_to_index;
		std::map<uint32_t, size_t> uniform_binding_to_index;
	public:
		VK_PipelineDescriptors(VK_Renderer *renderer, const VK_PipelineLayout &layout);
		~VK_PipelineDescriptors();
		void bind() const;
		void bind(VkCommandBuffer command_buffer, uint32_t frame) const;
		void unbind() const;
		void setUniform(const std::string &name, const void *data, int32_t frame);
		void setUniform(uint32_t binding, const void *data, int32_t frame);
		void setTexture(uint32_t binding, const Texture *texture, int32_t frame,uint32_t mip_level);
		void setTexture(const std::string &name, const Texture *texture, int32_t frame, uint32_t mip_level);
		void setTextureArray(const std::string &name, const Texture **textures, uint32_t texture_count, uint32_t frame, int32_t mip_level);
		void setTextureArray(uint32_t binding, const Texture **textures, uint32_t texture_count, uint32_t frame, int32_t mip_level);
		void setAccelerationStructure(uint32_t binding, const VK_TopLevelAccelerationStructure *acceleration_structure);
		void setAccelerationStructure(const std::string &name, const VK_TopLevelAccelerationStructure *acceleration_structure);
		void createDescriptorSets();
		void createDescriptorPool();

	};

}

