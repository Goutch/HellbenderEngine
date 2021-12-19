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

	enum UNIFORM_INPUT_TYPE {
		UNIFORM_INPUT_TYPE_BUFFER,
		UNIFORM_INPUT_TYPE_PUSH_CONSTANT,
		UNIFORM_INPUT_TYPE_TEXTURE_SAMPLER,
	};
	struct UniformInput {
		std::string name;
		UNIFORM_INPUT_TYPE type;
		uint32_t binding;
		VkShaderStageFlags stage;
		VkDeviceSize size;
		VkDeviceSize offset;
	};
	struct VertexInput {
		uint32_t location;
		uint32_t size;
		VkFormat format;
	};

	class VK_Shader : public Shader {
	private:
		const VK_Device *device;
		VkShaderModule handle = VK_NULL_HANDLE;
		std::vector<VertexInput> vertex_attribute_descriptions;
		std::vector<VkDescriptorSetLayoutBinding> layout_bindings;
		std::vector<VkPushConstantRange> push_constants_ranges;
		std::vector<UniformInput> inputs;
		SHADER_STAGE stage;
		VkShaderStageFlags vk_stage;

	public:

		VK_Shader(const VK_Device *device, const ShaderInfo &info);
		~VK_Shader();

		const VkShaderModule &getHandle() const;

		const std::vector<VkPushConstantRange> &getPushConstantRanges() const;

		const std::vector<UniformInput> &getInputs() const;
		SHADER_STAGE getStage() const override;
		const std::vector<VkDescriptorSetLayoutBinding> &getLayoutBindings() const;
		const std::vector<VertexInput> &getAttributeDescriptions() const;
	private:
		void load(const std::string &path);
		void setSource(const std::vector<uint32_t> &spirv);
		void reflect(const std::vector<uint32_t> &spirv);
	};
}

