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
		VkShaderStageFlags stage_flag;
	public:

		VK_Shader(const VK_Device *device);
		~VK_Shader();

		void setSource(const std::string &source, SHADER_STAGE stage) override;
		const VkShaderModule &getHandle() const;

		const std::vector<VkPushConstantRange> &getPushConstantRanges() const;

		const std::vector<ShaderInput> & getInputs() const;
		VkShaderStageFlags getStage() const;
		const std::vector<VkDescriptorSetLayoutBinding> &getLayoutBindings() const;

	};
}

