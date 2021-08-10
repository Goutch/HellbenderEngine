
#include "VK_Shader.h"
#include "VK_Device.h"
#include "ShaderCompiler.h"
#include "spirv_cross/spirv_cross_c.h"
#include "spirv_cross/spirv_cross.hpp"
#include "VK_CommandPool.h"
#include "spirv_cross.hpp"

namespace HBE {


	VK_Shader::~VK_Shader() {
		for (auto layout:descriptor_sets_layouts) {
			delete layout;
		}
		vkDestroyShaderModule(device->getHandle(), handle, nullptr);
	}

	VK_Shader::VK_Shader(const VK_Device *device) {
		this->device = device;
	}

	VkDeviceSize parseResourceSize(spirv_cross::Compiler &comp, spirv_cross::Resource &res) {
		VkDeviceSize size;
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


		spvc_context context = nullptr;
		spvc_context_create(&context);

		spvc_parsed_ir ir;
		spvc_context_parse_spirv(context, spirv.data(), spirv.size(), &ir);

		spvc_compiler compiler_glsl = nullptr;
		spvc_context_create_compiler(context, SPVC_BACKEND_GLSL, ir, SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler_glsl);

		spvc_resources resources;
		const spvc_reflected_resource *res_list = nullptr;
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

			descriptor_sets_layouts.emplace_back(new VK_DescriptorSetLayout(device, set, binding, (VkDeviceSize) size, stage_flag));

			printf("uniform %s at set = %u, binding = %u, size=%u\n", res_list[i].name, set, binding, size);
		}
		spvc_context_release_allocations(context);
		spvc_context_destroy(context);

	}

	const VkShaderModule &VK_Shader::getHandle() const {
		return handle;
	}

	const std::vector<VK_DescriptorSetLayout *> &VK_Shader::getDescriptorSetsLayouts() const {
		return descriptor_sets_layouts;
	}
}
