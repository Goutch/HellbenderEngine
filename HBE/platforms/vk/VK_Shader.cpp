
#include "VK_Shader.h"
#include "VK_Device.h"
#include "ShaderCompiler.h"
#include "spirv_cross/spirv_cross_c.h"
#include "spirv_cross/spirv_cross.hpp"
#include "VK_CommandPool.h"
#include "spirv_cross.hpp"

namespace HBE {
	VK_Shader::~VK_Shader() {
		vkDestroyShaderModule(device->getHandle(), handle, nullptr);
	}

	VK_Shader::VK_Shader(const VK_Device *device,const ShaderInfo& info) {
		this->device = device;
		this->stage=info.stage;
		switch (info.stage) {
			case SHADER_STAGE::SHADER_STAGE_COMPUTE:
				vk_stage = VK_SHADER_STAGE_COMPUTE_BIT;
				break;
			case SHADER_STAGE::SHADER_STAGE_VERTEX:
				vk_stage = VK_SHADER_STAGE_VERTEX_BIT;
				break;
			case SHADER_STAGE::SHADER_STAGE_FRAGMENT:
				vk_stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				break;
			case SHADER_STAGE::SHADER_STAGE_GEOMETRY:
				vk_stage = VK_SHADER_STAGE_GEOMETRY_BIT;
				break;
		}
		load(info.path);

	}

	void VK_Shader::setSource(const std::vector<uint32_t> &spirv) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = spirv.size() * sizeof(uint32_t);
		createInfo.pCode = reinterpret_cast<const uint32_t *>(spirv.data());

		if (vkCreateShaderModule(device->getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
			Log::error("failed to create shader module!");
		}
		reflect(spirv);
	}

	const VkShaderModule &VK_Shader::getHandle() const {
		return handle;
	}

	const std::vector<VkDescriptorSetLayoutBinding> &VK_Shader::getLayoutBindings() const {
		return layout_bindings;
	}

	const std::vector<VK_Shader::ShaderInput> &VK_Shader::getInputs() const {
		return inputs;
	}

	const std::vector<VkPushConstantRange> &VK_Shader::getPushConstantRanges() const {
		return push_constants_ranges;
	}

	SHADER_STAGE VK_Shader::getStage() const {
		return stage;
	}

	void VK_Shader::load(const std::string &path) {
		std::vector<char> source;
		getSource(path, source);
		std::vector<uint32_t> spirv;
		ShaderCompiler::GLSLToSpirV(source, spirv, stage);
		setSource(spirv);
	}

	void VK_Shader::reflect(const std::vector<uint32_t> &spirv) {
		//spirv cross: https://github.com/KhronosGroup/SPIRV-Cross/wiki/Reflection-API-user-guide
		spvc_context context = nullptr;
		spvc_context_create(&context);

		spvc_parsed_ir ir;
		spvc_context_parse_spirv(context, spirv.data(), spirv.size(), &ir);

		spvc_compiler compiler_glsl = nullptr;
		spvc_context_create_compiler(context, SPVC_BACKEND_GLSL, ir, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler_glsl);

		spvc_resources resources;
		const spvc_reflected_resource *uniform_list = nullptr;
		size_t uniform_count;
		spvc_compiler_create_shader_resources(compiler_glsl, &resources);
		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_UNIFORM_BUFFER, &uniform_list, &uniform_count);
		for (uint32_t i = 0; i < uniform_count; ++i) {
			VkDescriptorSetLayoutBinding layout_binding = {};
			layout_binding.binding = spvc_compiler_get_decoration(compiler_glsl, uniform_list[i].id, SpvDecorationBinding);
			layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layout_binding.stageFlags = vk_stage;
			layout_binding.descriptorCount = 1;//this is for array
			layout_binding.pImmutableSamplers = nullptr;
			layout_bindings.emplace_back(layout_binding);


			std::string name = spvc_compiler_get_name(compiler_glsl, uniform_list[i].id);


			uint32_t set = spvc_compiler_get_decoration(compiler_glsl, uniform_list[i].id, SpvDecorationDescriptorSet);

			size_t size;
			spvc_type type = spvc_compiler_get_type_handle(compiler_glsl, uniform_list[i].type_id);
			spvc_compiler_get_declared_struct_size(compiler_glsl, type, &size);


			inputs.emplace_back(ShaderInput{name, INPUT_TYPE::UNIFORM_BUFFER, layout_binding.binding,vk_stage,size});
		}

		const spvc_reflected_resource *push_constant_list = nullptr;
		size_t push_constants_count;
		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_PUSH_CONSTANT, &push_constant_list, &push_constants_count);
		for (uint32_t i = 0; i < push_constants_count; ++i) {


			size_t size;
			spvc_type type = spvc_compiler_get_type_handle(compiler_glsl, push_constant_list[i].type_id);
			spvc_compiler_get_declared_struct_size(compiler_glsl, type, &size);

			uint32_t binding = spvc_compiler_get_decoration(compiler_glsl, push_constant_list[i].id, SpvDecorationBinding);
			std::string name = spvc_compiler_get_name(compiler_glsl, push_constant_list[i].id);


			VkPushConstantRange push_constant;
			push_constant.size = size;
			push_constant.offset = spvc_compiler_get_decoration(compiler_glsl, push_constant_list[i].id, SpvDecorationOffset);
			push_constant.stageFlags = vk_stage;

			push_constants_ranges.emplace_back(push_constant);

			inputs.emplace_back(ShaderInput{name, INPUT_TYPE::PUSH_CONSTANT, binding,vk_stage,size,push_constant.offset});
		}
		const spvc_reflected_resource *texture_sampler_list = nullptr;
		size_t texture_sampler_count;

		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SAMPLED_IMAGE, &texture_sampler_list, &texture_sampler_count);

		for (uint32_t i = 0; i < texture_sampler_count; ++i) {
			VkDescriptorSetLayoutBinding layout_binding = {};
			layout_binding.binding = spvc_compiler_get_decoration(compiler_glsl, texture_sampler_list[i].id, SpvDecorationBinding);
			layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			layout_binding.stageFlags = vk_stage;
			layout_binding.descriptorCount = 1;//this is for array
			layout_binding.pImmutableSamplers = nullptr;
			layout_bindings.emplace_back(layout_binding);


			std::string name = spvc_compiler_get_name(compiler_glsl, texture_sampler_list[i].id);

			uint32_t set = spvc_compiler_get_decoration(compiler_glsl, texture_sampler_list[i].id, SpvDecorationDescriptorSet);


			inputs.emplace_back(ShaderInput{name, INPUT_TYPE::TEXTURE_SAMPLER, layout_binding.binding,vk_stage});
		}


		spvc_context_release_allocations(context);
		spvc_context_destroy(context);
	}
}
