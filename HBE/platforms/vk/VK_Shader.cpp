
#include "VK_Shader.h"
#include "core/utility/Log.h"
#include "VK_Renderer.h"
#include "VK_Device.h"
#include "ShaderCompiler.h"
#include "vulkan/vulkan.h"
#include "spirv_cross/spirv_cross_c.h"
#include "spirv_cross/spirv_cross.hpp"
#include "VK_CommandPool.h"
#include <utility>
#include "spirv_cross.hpp"

namespace HBE {


	VK_Shader::~VK_Shader() {
		for (auto descriptor_set_layout:descriptor_sets_layouts) {
			vkDestroyDescriptorSetLayout(device->getHandle(), descriptor_set_layout.second, nullptr);
		}
		descriptor_sets_layouts.clear();
		vkDestroyShaderModule(device->getHandle(), handle, nullptr);
	}

	VK_Shader::VK_Shader(const VK_Device *device) {
		this->device = device;

	}

	VkDeviceSize parseResourceSize(spirv_cross::Compiler &comp, spirv_cross::Resource &res) {
		VkDeviceSize size = 0;
		const spirv_cross::SPIRType &base_type = comp.get_type(res.base_type_id);
		const spirv_cross::SPIRType &type = comp.get_type(res.type_id);

		size = comp.get_declared_struct_size(type);
		return size;
	}

	void VK_Shader::setSource(const std::string &source, SHADER_STAGE stage) {
		this->type = stage;

		std::vector<uint32_t> spirv;
		ShaderCompiler::GLSLToSpirV(source, spirv, stage);

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = spirv.size() * sizeof(uint32_t);
		createInfo.pCode = reinterpret_cast<const uint32_t *>(spirv.data());

		if (vkCreateShaderModule(device->getHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS) {
			Log::error("failed to create shader module!");
		}



		//spirv cross: https://github.com/KhronosGroup/SPIRV-Cross/wiki/Reflection-API-user-guide


		spvc_context context = NULL;
		spvc_context_create(&context);

		spvc_parsed_ir ir;
		spvc_context_parse_spirv(context, spirv.data(), spirv.size(), &ir);

		spvc_compiler compiler_glsl = NULL;
		spvc_context_create_compiler(context, SPVC_BACKEND_GLSL, ir, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler_glsl);

		spvc_resources resources;
		const spvc_reflected_resource *res_list = NULL;
		size_t res_count;
		spvc_compiler_create_shader_resources(compiler_glsl, &resources);
		spvc_resources_get_resource_list_for_type(resources, SPVC_RESOURCE_TYPE_UNIFORM_BUFFER, &res_list, &res_count);
		for (int i = 0; i < res_count; ++i) {
			uint32_t set = spvc_compiler_get_decoration(compiler_glsl, res_list[i].id, SpvDecorationDescriptorSet);
			uint32_t binding = spvc_compiler_get_decoration(compiler_glsl, res_list[i].id, SpvDecorationBinding);
			size_t size;
			spvc_type type = spvc_compiler_get_type_handle(compiler_glsl, res_list[i].type_id);
			spvc_compiler_get_declared_struct_size(compiler_glsl, type, &size);

			VkShaderStageFlags stage_flag;
			switch (stage) {

				case SHADER_STAGE::COMPUTE:
					stage_flag = VK_SHADER_STAGE_COMPUTE_BIT;
					break;
				case SHADER_STAGE::VERTEX:
					stage_flag = VK_SHADER_STAGE_VERTEX_BIT;
					break;
				case SHADER_STAGE::FRAGMENT:
					stage_flag = VK_SHADER_STAGE_FRAGMENT_BIT;
					break;
				case SHADER_STAGE::GEOMETRY:
					stage_flag = VK_SHADER_STAGE_GEOMETRY_BIT;
					break;
			}


			VkDescriptorSetLayoutBinding uboLayoutBinding{};
			uboLayoutBinding.binding = binding;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.stageFlags = stage_flag;
			uboLayoutBinding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = 1;
			layoutInfo.pBindings = &uboLayoutBinding;

			bindings.emplace(res_list[i].name, binding);
			VkDescriptorSetLayout layout;

			vkCreateDescriptorSetLayout(device->getHandle(), &layoutInfo, nullptr, &layout);
			descriptor_sets_layouts.emplace(binding, layout);

			uniform_sizes.emplace(binding, size);
			printf("uniform %s at set = %u, binding = %u, size=%u\n", res_list[i].name, set, binding, size);
		}
		spvc_context_release_allocations(context);
		spvc_context_destroy(context);


		/*spirv_cross::Compiler *compiler = new spirv_cross::Compiler(move(spirv)); // const uint32_t *, size_t interface is also available.
		spirv_cross::Compiler &comp = *compiler;

		// The SPIR-V is now parsed, and we can perform reflection on it.
		spirv_cross::ShaderResources res = comp.get_shader_resources();

		// Get all uniform buffers in the shader.
		descriptor_sets_layouts.clear();
		uniform_sizes.clear();
		bindings.clear();
		for (auto &uniform : res.uniform_buffers) {
			uint32_t set = comp.get_decoration(uniform.id, spv::DecorationDescriptorSet);
			uint32_t binding = comp.get_decoration(uniform.id, spv::DecorationBinding);

			VkShaderStageFlags stage_flag;
			switch (stage) {

				case SHADER_STAGE::COMPUTE:
					stage_flag = VK_SHADER_STAGE_COMPUTE_BIT;
					break;
				case SHADER_STAGE::VERTEX:
					stage_flag = VK_SHADER_STAGE_VERTEX_BIT;
					break;
				case SHADER_STAGE::FRAGMENT:
					stage_flag = VK_SHADER_STAGE_FRAGMENT_BIT;
					break;
				case SHADER_STAGE::GEOMETRY:
					stage_flag = VK_SHADER_STAGE_GEOMETRY_BIT;
					break;
			}


			VkDescriptorSetLayoutBinding uboLayoutBinding{};
			uboLayoutBinding.binding = binding;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.stageFlags = stage_flag;
			uboLayoutBinding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = 1;
			layoutInfo.pBindings = &uboLayoutBinding;

			bindings.emplace(uniform.name, binding);
			VkDescriptorSetLayout layout;

			vkCreateDescriptorSetLayout(device->getHandle(), &layoutInfo, nullptr, &layout);
			descriptor_sets_layouts.emplace(binding, layout);
			VkDeviceSize size = parseResourceSize(comp, uniform);
			uniform_sizes.emplace(binding, size);
			printf("uniform %s at set = %u, binding = %u, size=%llu\n", uniform.name.c_str(), set, binding, size);
		}

		delete compiler;*/
	}

	const VkShaderModule &VK_Shader::getHandle() const {
		return handle;
	}

	std::unordered_map<uint32_t, VkDescriptorSetLayout> VK_Shader::getDescriptorSetsLayouts() const {
		return descriptor_sets_layouts;
	}

	VkDeviceSize VK_Shader::getBindingSize(uint32_t binding) const {
		return uniform_sizes.at(binding);
	}


}
