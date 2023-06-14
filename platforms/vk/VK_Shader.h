#pragma once

#include "core/resource/Shader.h"
#include "vulkan/vulkan.h"
#include "string"
#include "VK_Buffer.h"

namespace HBE {
	class VK_Renderer;

	class VK_Device;

	class VK_CommandPool;

	struct VK_VertexAttributeInfo {
		uint32_t location;
		uint32_t size;
		VkFormat format;
	};

	struct VK_DescriptorInfo {
		std::string name;
		VkDeviceSize size = 0;
		uint32_t set = 0;
		VkDescriptorSetLayoutBinding layout_binding{};
		bool variable_size = false;
	};

	struct VK_PushConstantInfo {
		std::string name;
		VkPushConstantRange push_constant_range;
	};

	class VK_Shader : public Shader {
	private:
		const VK_Device *device;
		VkShaderModule handle = VK_NULL_HANDLE;
		std::vector<VK_VertexAttributeInfo> vertex_inputs;
		std::vector<VK_DescriptorInfo> uniforms;
		std::vector<VK_PushConstantInfo> push_constants;

		SHADER_STAGE stage;
		VkShaderStageFlagBits vk_stage;

	public:

		VK_Shader(const VK_Device *device, const ShaderInfo &info);

		~VK_Shader();

		const VkShaderModule &getHandle() const;

		const std::vector<VK_PushConstantInfo> &getPushConstants() const;

		const std::vector<VK_DescriptorInfo> &getDescriptorInfos() const;

		const std::vector<VK_VertexAttributeInfo> &getVertexInputs() const;

		SHADER_STAGE getStage() const override;

		VkShaderStageFlagBits getVkStage() const;

	private:
		void load(const std::string &path);

		void setSource(const std::vector<uint32_t> &spirv);

		void reflect(const std::vector<uint32_t> &spirv);

		void reflect_c(const std::vector<uint32_t> &spirv);
	};
}

