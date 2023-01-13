
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
			case SHADER_STAGE::SHADER_STAGE_RAY_GEN:
				vk_stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
				break;
			case SHADER_STAGE::SHADER_STAGE_RAY_MISS:
				vk_stage = VK_SHADER_STAGE_MISS_BIT_KHR;
				break;
			case SHADER_STAGE::SHADER_STAGE_CLOSEST_HIT:
				vk_stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
				break;
			case SHADER_STAGE::SHADER_STAGE_ANY_HIT:
				vk_stage = VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
				break;
			case SHADER_STAGE::SHADER_STAGE_INTERSECTION:
				vk_stage = VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
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

	SHADER_STAGE VK_Shader::getStage() const {
		return stage;
	}

	void VK_Shader::load(const std::string &path) {
		std::string source;
		getSource(path, source);
		std::vector<uint32_t> spirv;
		ShaderCompiler::GLSLToSpirV(source.c_str(), source.size(), spirv, stage, path);
		setSource(spirv);
	}

	void VK_Shader::reflect(const std::vector<uint32_t> &spirv) {

		VkPhysicalDeviceLimits limits = device->getPhysicalDevice().getProperties().limits;
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

		//----------------------------------------------PUSH CONSTANTS------------------------------------------------
		const spvc_reflected_resource *push_constant_list = nullptr;
		size_t push_constants_count;
		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_PUSH_CONSTANT, &push_constant_list, &push_constants_count);
		for (uint32_t i = 0; i < push_constants_count; ++i) {
			size_t size;
			spvc_type type = spvc_compiler_get_type_handle(compiler_glsl, push_constant_list[i].type_id);
			spvc_compiler_get_declared_struct_size(compiler_glsl, type, &size);

			HB_ASSERT(size <= limits.maxPushConstantsSize, "Push constant size is too big!");

			//uint32_t binding = spvc_compiler_get_decoration(compiler_glsl, push_constant_list[i].id, SpvDecorationBinding);
			std::string name = spvc_compiler_get_name(compiler_glsl, push_constant_list[i].id);

			VK_PushConstantInfo push_constant_info{};
			push_constant_info.name = name;

			VkPushConstantRange push_constant_range{};
			push_constant_range.size = size;
			push_constant_range.offset = spvc_compiler_get_decoration(compiler_glsl, push_constant_list[i].id, SpvDecorationOffset);
			push_constant_range.stageFlags = vk_stage;

			push_constant_info.push_constant_range = push_constant_range;
			push_constants.emplace_back(push_constant_info);
		}

		std::sort(push_constants.begin(), push_constants.end(),
				  [](const VK_PushConstantInfo &a, const VK_PushConstantInfo &b) -> bool {
					  return a.push_constant_range.offset < b.push_constant_range.offset;
				  });

		//-------------------------------------------------UNIFORM_BUFFERS-----------------------------
		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_UNIFORM_BUFFER, &uniform_list, &uniform_count);
		for (uint32_t i = 0; i < uniform_count; ++i) {

			std::string name = spvc_compiler_get_name(compiler_glsl, uniform_list[i].id);
			size_t size;
			spvc_type type = spvc_compiler_get_type_handle(compiler_glsl, uniform_list[i].type_id);
			spvc_compiler_get_declared_struct_size(compiler_glsl, type, &size);

			VK_UniformInfo uniform_info{};
			HB_ASSERT(size <= limits.maxUniformBufferRange, "Uniform buffer size is too big!");

			VkDescriptorSetLayoutBinding layout_binding = {};
			layout_binding.binding = spvc_compiler_get_decoration(compiler_glsl, uniform_list[i].id, SpvDecorationBinding);
			layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layout_binding.stageFlags = vk_stage;
			layout_binding.descriptorCount = 1;//this is for array
			layout_binding.pImmutableSamplers = nullptr;

			uniform_info.layout_binding = layout_binding;
			uniform_info.name = name;
			uniform_info.size = size;


			uniforms.emplace_back(uniform_info);
		}


		//----------------------------------------------------------TEXTURE SAMPLERS----------------------------------------------------------
		const spvc_reflected_resource *texture_sampler_list = nullptr;
		size_t texture_sampler_count;

		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SAMPLED_IMAGE, &texture_sampler_list, &texture_sampler_count);

		for (uint32_t i = 0; i < texture_sampler_count; ++i) {
			std::string name = spvc_compiler_get_name(compiler_glsl, texture_sampler_list[i].id);


			spvc_type type = spvc_compiler_get_type_handle(compiler_glsl, texture_sampler_list[i].type_id);

			uint32_t descriptor_count = 1;
			if (spvc_type_get_image_arrayed(type)) {

				descriptor_count = spvc_type_get_array_dimension(type, 0);
			}
			VK_UniformInfo uniform_info{};
			if (descriptor_count == 0) {
				uniform_info.variable_size = true;
				descriptor_count = limits.maxPerStageDescriptorSamplers;
			}
			VkDescriptorSetLayoutBinding layout_binding = {};
			layout_binding.binding = spvc_compiler_get_decoration(compiler_glsl, texture_sampler_list[i].id, SpvDecorationBinding);
			layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			layout_binding.stageFlags = vk_stage;
			layout_binding.descriptorCount = descriptor_count;//this is for arrays
			layout_binding.pImmutableSamplers = nullptr;

			uniform_info.layout_binding = layout_binding;
			uniform_info.name = name;
			uniform_info.size = 0;

			uniforms.emplace_back(uniform_info);
		}

		//--------------------------------------------------------IMAGE----------------------------------------

		const spvc_reflected_resource *separate_image_list;
		size_t separate_image_count;

		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_SEPARATE_IMAGE, &separate_image_list, &separate_image_count);
		for (uint32_t i = 0; i < separate_image_count; ++i) {
			std::string name = spvc_compiler_get_name(compiler_glsl, separate_image_list[i].id);

			spvc_type type = spvc_compiler_get_type_handle(compiler_glsl, separate_image_list[i].type_id);

			uint32_t descriptor_count = 1;
			if (spvc_type_get_image_arrayed(type)) {

				descriptor_count = spvc_type_get_array_dimension(type, 0);
			}
			VK_UniformInfo uniform_info{};
			if (descriptor_count == 0) {
				uniform_info.variable_size = true;
				descriptor_count = limits.maxPerStageDescriptorSampledImages;
			}


			VkDescriptorSetLayoutBinding layout_binding = {};
			layout_binding.binding = spvc_compiler_get_decoration(compiler_glsl, separate_image_list[i].id, SpvDecorationBinding);
			layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			layout_binding.stageFlags = vk_stage;
			layout_binding.descriptorCount = descriptor_count;//this is for array
			layout_binding.pImmutableSamplers = nullptr;

			uniform_info.layout_binding = layout_binding;
			uniform_info.name = name;
			uniform_info.size = 0;

			uniforms.emplace_back(uniform_info);
		}
		//--------------------------------------------------------STORAGE_IMAGES----------------------------------------

		const spvc_reflected_resource *storage_image_list;
		size_t storage_image_count;

		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_STORAGE_IMAGE, &storage_image_list, &storage_image_count);
		for (uint32_t i = 0; i < storage_image_count; ++i) {

			std::string name = spvc_compiler_get_name(compiler_glsl, storage_image_list[i].id);

			spvc_type type = spvc_compiler_get_type_handle(compiler_glsl, storage_image_list[i].type_id);

			uint32_t descriptor_count = 1;
			uint32_t array_dimension = spvc_type_get_num_array_dimensions(type);
			if (array_dimension == 1) {
				descriptor_count = spvc_type_get_array_dimension(type, 0);
			}
			VK_UniformInfo uniform_info{};
			if (descriptor_count == 0) {
				uniform_info.variable_size = true;
				descriptor_count = limits.maxPerStageDescriptorStorageImages;
			}


			VkDescriptorSetLayoutBinding layout_binding = {};
			layout_binding.binding = spvc_compiler_get_decoration(compiler_glsl, storage_image_list[i].id, SpvDecorationBinding);
			layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			layout_binding.stageFlags = vk_stage;
			layout_binding.descriptorCount = descriptor_count;//this is for array
			layout_binding.pImmutableSamplers = nullptr;

			uniform_info.layout_binding = layout_binding;
			uniform_info.name = name;
			uniform_info.size = 0;

			uniforms.emplace_back(uniform_info);
		}


		//----------------------------------------------------------ACCELERATION_STRUCTURES----------------------------------------------------------
		const spvc_reflected_resource *acceleration_structure_list;
		size_t acceleration_structure_count;

		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_ACCELERATION_STRUCTURE, &acceleration_structure_list, &acceleration_structure_count);
		for (uint32_t i = 0; i < acceleration_structure_count; ++i) {

			std::string name = spvc_compiler_get_name(compiler_glsl, acceleration_structure_list[i].id);

			VK_UniformInfo uniform_info{};

			VkDescriptorSetLayoutBinding layout_binding = {};
			layout_binding.binding = spvc_compiler_get_decoration(compiler_glsl, acceleration_structure_list[i].id, SpvDecorationBinding);
			layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
			layout_binding.stageFlags = vk_stage;
			layout_binding.descriptorCount = 1;//this is for array
			layout_binding.pImmutableSamplers = nullptr;

			uniform_info.layout_binding = layout_binding;
			uniform_info.name = name;
			uniform_info.size = 0;

			uniforms.emplace_back(uniform_info);
		}

		//----------------------------------------------------------STORAGE BUFFERS----------------------------------------------------------
		const spvc_reflected_resource *buffer_list = nullptr;
		size_t buffer_count = 0;
		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_STORAGE_BUFFER, &buffer_list, &buffer_count);

		for (uint32_t i = 0; i < buffer_count; ++i) {

			uint32_t descriptor_count = 1;

			spvc_type type = spvc_compiler_get_type_handle(compiler_glsl, buffer_list[i].type_id);

			uint32_t array_dimension = spvc_type_get_num_array_dimensions(type);

			if (array_dimension == 1) {

				descriptor_count = spvc_type_get_array_dimension(type, 0);
			}

			std::string name = spvc_compiler_get_name(compiler_glsl, buffer_list[i].id);

			VK_UniformInfo uniform_info{};
			if (descriptor_count == 0) {
				uniform_info.variable_size = true;
				descriptor_count = limits.maxPerStageDescriptorStorageBuffers;
			}


			VkDescriptorSetLayoutBinding layout_binding = {};

			layout_binding.binding = spvc_compiler_get_decoration(compiler_glsl, buffer_list[i].id, SpvDecorationBinding);
			layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			layout_binding.stageFlags = vk_stage;
			layout_binding.descriptorCount = descriptor_count;//this is for array
			layout_binding.pImmutableSamplers = nullptr;

			uniform_info.layout_binding = layout_binding;
			uniform_info.name = name;
			uniform_info.size = 0;

			uniforms.emplace_back(uniform_info);
		}
		//----------------------------------------------------------STORAGE TEXEL BUFFERS----------------------------------------------------------
		const spvc_reflected_resource *texel_buffer_list = nullptr;
		size_t texel_buffer_count = 0;
		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_STORAGE_BUFFER, &texel_buffer_list, &buffer_count);

		for (uint32_t i = 0; i < buffer_count; ++i) {

			uint32_t descriptor_count = 1;

			spvc_type type = spvc_compiler_get_type_handle(compiler_glsl, texel_buffer_list[i].type_id);

			uint32_t array_dimension = spvc_type_get_num_array_dimensions(type);

			if (array_dimension == 1) {

				descriptor_count = spvc_type_get_array_dimension(type, 0);
			}

			std::string name = spvc_compiler_get_name(compiler_glsl, texel_buffer_list[i].id);

			VK_UniformInfo uniform_info{};
			if (descriptor_count == 0) {
				uniform_info.variable_size = true;
				descriptor_count = limits.maxPerStageDescriptorStorageBuffers;
			}


			VkDescriptorSetLayoutBinding layout_binding = {};

			layout_binding.binding = spvc_compiler_get_decoration(compiler_glsl, texel_buffer_list[i].id, SpvDecorationBinding);
			layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
			layout_binding.stageFlags = vk_stage;
			layout_binding.descriptorCount = descriptor_count;//this is for array
			layout_binding.pImmutableSamplers = nullptr;

			uniform_info.layout_binding = layout_binding;
			uniform_info.name = name;
			uniform_info.size = 0;

			uniforms.emplace_back(uniform_info);
		}

		//----------------------------------------------------------VERTEX INPUTS-----------------------------
		const spvc_reflected_resource *vertex_input_list = nullptr;
		size_t vertex_inputs_count;
		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_STAGE_INPUT, &vertex_input_list, &vertex_inputs_count);

		for (size_t i = 0; i < vertex_inputs_count; ++i) {

			spvc_type type = spvc_compiler_get_type_handle(compiler_glsl, vertex_input_list[i].type_id);
			spvc_basetype basetype = spvc_type_get_basetype(type);
			size_t size;
			size = spvc_type_get_vector_size(type);
			size_t num_col = spvc_type_get_columns(type);
			uint32_t location = spvc_compiler_get_decoration(compiler_glsl, vertex_input_list[i].id, SpvDecorationLocation);
			for (size_t j = 0; j < num_col; ++j) {
				VK_VertexInputInfo attribute_description{};
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
				vertex_inputs.emplace_back(attribute_description);
				location++;
			}

		}


		std::sort(uniforms.begin(), uniforms.end(),
				  [](const VK_UniformInfo &a, const VK_UniformInfo &b) -> bool {
					  return a.layout_binding.binding < b.layout_binding.binding;
				  });
		std::sort(vertex_inputs.begin(), vertex_inputs.end(),
				  [](const VK_VertexInputInfo &a, const VK_VertexInputInfo &b) -> bool {
					  return a.location < b.location;
				  });


		spvc_context_release_allocations(context);
		spvc_context_destroy(context);
	}


	VkShaderStageFlagBits VK_Shader::getVkStage() const {
		return vk_stage;
	}

	const std::vector<VK_PushConstantInfo> &VK_Shader::getPushConstants() const {
		return push_constants;
	}

	const std::vector<VK_UniformInfo> &VK_Shader::getUniforms() const {
		return uniforms;
	}

	const std::vector<VK_VertexInputInfo> &VK_Shader::getVertexInputs() const {
		return vertex_inputs;
	}
}
