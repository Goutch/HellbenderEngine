#pragma once

#include "core/resource/Shader.h"
#include "vulkan/vulkan.h"
#include "string"
#include "VK_Buffer.h"
#include "VK_CONSTANTS.h"
#include "VK_DescriptorSetLayout.h"

namespace HBE {
	class VK_Renderer;

	class VK_Device;

	class VK_CommandPool;

	class VK_Shader : public Shader {
		const VK_Device *device;
		VkShaderModule handle = VK_NULL_HANDLE;
		std::vector<VK_DescriptorSetLayout*> descriptor_sets_layouts;

	public:

		VK_Shader(const VK_Device *device);
		~VK_Shader();

		void setSource(const std::string &source, SHADER_STAGE stage) override;
		const VkShaderModule &getHandle() const;

		const std::vector<VK_DescriptorSetLayout*> & getDescriptorSetsLayouts() const;


	};
}

