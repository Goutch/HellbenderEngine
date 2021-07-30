#pragma once

#include "core/resource/Shader.h"
#include "vulkan/vulkan.h"
#include "string"
#include "VK_Buffer.h"
#include "VK_CONSTANTS.h"


namespace HBE {
	class VK_Renderer;

	class VK_Device;

	class VK_CommandPool;

	class VK_Shader : public Shader {
		const VK_Device *device;
		VkShaderModule handle;
		std::unordered_map<uint32_t, VkDescriptorSetLayout> descriptor_sets_layouts;
		std::unordered_map<uint32_t, VkDeviceSize> uniform_sizes;
		std::unordered_map<std::string, uint32_t> bindings;

	public:

		VK_Shader(const VK_Device *device);
		~VK_Shader();

		void setSource(const std::string &source, SHADER_STAGE stage) override;
		const VkShaderModule &getHandle() const;

		std::unordered_map<uint32_t, VkDescriptorSetLayout> getDescriptorSetsLayouts() const;
		VkDeviceSize getBindingSize(uint32_t binding) const;

	};
}

