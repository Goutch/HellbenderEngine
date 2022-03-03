
#include "VK_Shader.h"
#include "VK_Device.h"
#include "ShaderCompiler.h"
#include "spirv_cross.hpp"
#include "spirv_cross_c.h"
#include "VK_CommandPool.h"
#include "spirv_cross.hpp"

namespace HBE {
	VK_Shader::~VK_Shader() {
		vkDestroyShaderModule(device->getHandle(), handle, nullptr);
	}

	VK_Shader::VK_Shader(const VK_Device *device, const ShaderInfo &info) {
		this->device = device;
		this->stage = info.stage;
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
			case SHADER_STAGE_NONE:
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

	const std::vector<UniformInput> &VK_Shader::getInputs() const {
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

		//-------------------------------------------------UNIFORM_BUFFERS-----------------------------
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
			size_t size;
			spvc_type type = spvc_compiler_get_type_handle(compiler_glsl, uniform_list[i].type_id);
			spvc_compiler_get_declared_struct_size(compiler_glsl, type, &size);

			inputs.emplace_back(UniformInput{name, UNIFORM_INPUT_TYPE_BUFFER, layout_binding.binding, vk_stage, size});
		}

		//----------------------------------------------PUSH CONSTANTS------------------------------------------------
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

			inputs.emplace_back(UniformInput{name, UNIFORM_INPUT_TYPE_PUSH_CONSTANT, binding, vk_stage, size, push_constant.offset});
		}


		//----------------------------------------------------------TEXTURE SAMPLERS----------------------------------------------------------
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
			inputs.emplace_back(UniformInput{name, UNIFORM_INPUT_TYPE_TEXTURE_SAMPLER, layout_binding.binding, vk_stage});
		}

		//--------------------------------------------------------IMAGE----------------------------------------

		const spvc_reflected_resource *separate_image_list;
		size_t separate_image_count;

		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SEPARATE_IMAGE, &separate_image_list, &separate_image_count);
		for (uint32_t i = 0; i < separate_image_count; ++i) {
			VkDescriptorSetLayoutBinding layout_binding = {};
			layout_binding.binding = spvc_compiler_get_decoration(compiler_glsl, separate_image_list[i].id, SpvDecorationBinding);
			layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			layout_binding.stageFlags = vk_stage;
			layout_binding.descriptorCount = 1;//this is for array
			layout_binding.pImmutableSamplers = nullptr;
			layout_bindings.emplace_back(layout_binding);


			std::string name = spvc_compiler_get_name(compiler_glsl, separate_image_list[i].id);
			inputs.emplace_back(UniformInput{name, UNIFORM_INPUT_TYPE_IMAGE, layout_binding.binding, vk_stage});
		}
		//--------------------------------------------------------STORAGE_IMAGES----------------------------------------

		const spvc_reflected_resource *storage_image_list;
		size_t storage_image_count;

		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_STORAGE_IMAGE, &storage_image_list, &storage_image_count);
		for (uint32_t i = 0; i < storage_image_count; ++i) {
			VkDescriptorSetLayoutBinding layout_binding = {};
			layout_binding.binding = spvc_compiler_get_decoration(compiler_glsl, storage_image_list[i].id, SpvDecorationBinding);
			layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			layout_binding.stageFlags = vk_stage;
			layout_binding.descriptorCount = 1;//this is for array
			layout_binding.pImmutableSamplers = nullptr;
			layout_bindings.emplace_back(layout_binding);


			std::string name = spvc_compiler_get_name(compiler_glsl, storage_image_list[i].id);
			inputs.emplace_back(UniformInput{name, UNIFORM_INPUT_TYPE_IMAGE, layout_binding.binding, vk_stage});
		}
		//----------------------------------------------------------VERTEX INPUTS-----------------------------
		const spvc_reflected_resource *vertex_inputs = nullptr;
		size_t vertex_inputs_count;
		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_STAGE_INPUT, &vertex_inputs, &vertex_inputs_count);

		for (size_t i = 0; i < vertex_inputs_count; ++i) {

			spvc_type type = spvc_compiler_get_type_handle(compiler_glsl, vertex_inputs[i].type_id);
			spvc_basetype basetype = spvc_type_get_basetype(type);
			size_t size;
			size = spvc_type_get_vector_size(type);
			size_t num_col = spvc_type_get_columns(type);
			uint32_t location = spvc_compiler_get_decoration(compiler_glsl, vertex_inputs[i].id, SpvDecorationLocation);
			for (size_t j = 0; j < num_col; ++j) {
				VertexInput attribute_description{};
				attribute_description.location = location;
				attribute_description.size = size * 4;
				switch (size) {
					case 1:
						if (basetype == spvc_basetype::SPVC_BASETYPE_FP32)
							attribute_description.format = VK_FORMAT_R32_SFLOAT;
						else if (basetype == spvc_basetype::SPVC_BASETYPE_UINT32)
							attribute_description.format = VK_FORMAT_R32_UINT;
						break;
					case 2:
						if (basetype == spvc_basetype::SPVC_BASETYPE_FP32)
							attribute_description.format = VK_FORMAT_R32G32_SFLOAT;
						else if (basetype == spvc_basetype::SPVC_BASETYPE_UINT32)
							attribute_description.format = VK_FORMAT_R32G32_UINT;
						break;
					case 3:
						if (basetype == spvc_basetype::SPVC_BASETYPE_FP32)
							attribute_description.format = VK_FORMAT_R32G32B32_SFLOAT;
						else if (basetype == spvc_basetype::SPVC_BASETYPE_UINT32)
							attribute_description.format = VK_FORMAT_R32G32B32_UINT;
						break;
					case 4:
						if (basetype == spvc_basetype::SPVC_BASETYPE_FP32)
							attribute_description.format = VK_FORMAT_R32G32B32A32_SFLOAT;
						else if (basetype == spvc_basetype::SPVC_BASETYPE_UINT32)
							attribute_description.format = VK_FORMAT_R32G32B32A32_UINT;
						break;
				}
				vertex_attribute_descriptions.emplace_back(attribute_description);
				Log::message(std::string(vertex_inputs[i].name) +
							 " location:" + std::to_string(location) +
							 " size:" + std::to_string(size * 4));
				location++;
			}

		}
		std::sort(vertex_attribute_descriptions.begin(), vertex_attribute_descriptions.end(),
				  [](const VertexInput &a, const VertexInput &b) -> bool {
					  return a.location < b.location;
				  });


		spvc_context_release_allocations(context);
		spvc_context_destroy(context);
	}

	const std::vector<VertexInput> &VK_Shader::getAttributeDescriptions() const {
		return vertex_attribute_descriptions;
	}

	VkShaderStageFlags VK_Shader::getVkStage() const{
		return vk_stage;
	}
}
