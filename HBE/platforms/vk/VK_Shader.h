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
	public:
		enum INPUT_TYPE {
			UNIFORM_BUFFER,
			PUSH_CONSTANT,
			TEXTURE_SAMPLER,
		};
		struct ShaderInput {

			std::string name;
			INPUT_TYPE type;
			uint32_t binding;
			VkShaderStageFlags stage;
			VkDeviceSize size;
			VkDeviceSize offset;
		};
	private:
		const VK_Device *device;
		VkShaderModule handle = VK_NULL_HANDLE;
		std::vector<VkDescriptorSetLayoutBinding> layout_bindings;
		std::vector<VkPushConstantRange> push_constants_ranges;
		std::vector<ShaderInput> inputs;
		SHADER_STAGE stage;
		VkShaderStageFlags vk_stage;

	public:

		VK_Shader(const VK_Device *device,const ShaderInfo& info);
		~VK_Shader();

		const VkShaderModule &getHandle() const;

		const std::vector<VkPushConstantRange> &getPushConstantRanges() const;

		const std::vector<ShaderInput> &getInputs() const;
		SHADER_STAGE getStage() const override;
		const std::vector<VkDescriptorSetLayoutBinding> &getLayoutBindings() const;

	private:
		void load(const std::string &path);
		void setSource(const std::vector<uint32_t> &spirv);
		void reflect(const std::vector<uint32_t> &spirv);
	};
}

