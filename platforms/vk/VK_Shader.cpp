
#include "VK_Shader.h"
#include "VK_Device.h"
#include "ShaderCompiler.h"
#include "spirv_cross.hpp"
#include "spirv_cross_c.h"
#include "VK_CommandPool.h"
#include "spirv_glsl.hpp"

namespace HBE {
	VK_Shader::~VK_Shader() {
		vkDestroyShaderModule(device->getHandle(), handle, nullptr);
	}

	VK_Shader::VK_Shader(const VK_Device *device, const ShaderInfo &info) {
		this->info = info;
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
		load();

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

	void VK_Shader::load() {
		std::string source;
		getSource(info.path, source);
		std::vector<uint32_t> spirv;
		ShaderCompiler::GLSLToSpirV(source.c_str(), source.size(), spirv, stage, info.path, info.preamble);
		setSource(spirv);
	}

	VK_DescriptorInfo
	generateDescriptorInfo(VkShaderStageFlagBits stage, VkDescriptorType descriptor_type, spirv_cross::CompilerGLSL &glsl, spirv_cross::Resource &resource, VkPhysicalDeviceLimits limits) {
		std::string name = glsl.get_name(resource.id);
		uint32_t set_index = glsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
		uint32_t descriptor_count = 1;
		bool variable_size = false;
		size_t size = 0;

		spirv_cross::SPIRType type = glsl.get_type_from_variable(resource.id);


		if (type.basetype == spirv_cross::SPIRType::Struct) {
			size = glsl.get_declared_struct_size(type);
			//	if (type.member_types.size() > 0) {
			//		const spirv_cross::SPIRType &last_member_type = glsl.get_type(type.member_types.back());
			//		if (last_member_type.array.size() == 1 && last_member_type.array[0] == 0) {
			//			variable_size = true;
			//		}
			//	}
		}

		if (!type.array.empty()) {
			descriptor_count = type.array[0];
			if (descriptor_count <= 0) {
				variable_size = true;
			}
		}
		spirv_cross::TypeID parent_type_id = type.parent_type;
		while (parent_type_id != 0) {
			spirv_cross::SPIRType parent_type = glsl.get_type(parent_type_id);
			if (!parent_type.array.empty()) {
				descriptor_count = parent_type.array[0];
				if (descriptor_count <= 0) {
					variable_size = true;
				}
			}

			parent_type_id = parent_type.parent_type;
		}


		if (variable_size) {
			switch (descriptor_type) {
				case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
				case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
					descriptor_count = limits.maxPerStageDescriptorSampledImages;
					break;

				case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
				case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
					descriptor_count = limits.maxPerStageDescriptorStorageImages;
					break;
				case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
					descriptor_count = limits.maxPerStageDescriptorStorageBuffers;
					break;
				case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
					descriptor_count = limits.maxPerStageDescriptorUniformBuffers;
					break;

			}
		}

		VkDescriptorSetLayoutBinding layout_binding = {};
		layout_binding.
				binding = glsl.get_decoration(resource.id, spv::DecorationBinding);
		layout_binding.
				descriptorType = descriptor_type;
		layout_binding.
				stageFlags = stage;
		layout_binding.
				descriptorCount = descriptor_count; //this is for array
		layout_binding.
				pImmutableSamplers = nullptr;


		VK_DescriptorInfo descriptor_info{};
		descriptor_info.
				layout_binding = layout_binding;
		descriptor_info.
				descriptor_set = set_index;
		descriptor_info.
				name = name;
		descriptor_info.
				size = size;
		descriptor_info.
				variable_size = variable_size;
		return
				descriptor_info;
	}

//does what the relect_c function does but with the c++ api
	void VK_Shader::reflect(const std::vector<uint32_t> &spirv) {
		VkPhysicalDeviceLimits limits = device->getPhysicalDevice().getProperties().limits;
		spirv_cross::CompilerGLSL glsl(std::move(spirv));
		spirv_cross::ShaderResources resources = glsl.get_shader_resources();
		auto used_variables = glsl.get_active_interface_variables();

		//----------------------------------------------UNIFORM BUFFERS------------------------------------------------
		for (auto &ub: resources.uniform_buffers) {
			size_t size = glsl.get_declared_struct_size(glsl.get_type(ub.base_type_id));
			HB_ASSERT(size <= device->getPhysicalDevice().getProperties().limits.maxUniformBufferRange, "Uniform buffer size is too big!");

			VK_DescriptorInfo uniform_info = generateDescriptorInfo(vk_stage, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, glsl, ub, limits);
			uniforms.emplace_back(uniform_info);
		}

		//----------------------------------------------------------TEXTURE SAMPLERS----------------------------------------------------------
		for (auto &sampler: resources.sampled_images) {
			if (used_variables.find(sampler.id) == used_variables.end()) {
				//continue;
			}
			VK_DescriptorInfo uniform_info = generateDescriptorInfo(vk_stage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, glsl, sampler, limits);
			uniforms.emplace_back(uniform_info);
		}

		//----------------------------------------------STORAGE BUFFERS------------------------------------------------
		for (auto &sb: resources.storage_buffers) {
			if (used_variables.find(sb.id) == used_variables.end()) {
				//continue;
			}
			size_t size = glsl.get_declared_struct_size(glsl.get_type(sb.base_type_id));
			HB_ASSERT(size <= device->getPhysicalDevice().getProperties().limits.maxStorageBufferRange, "Storage buffer size is too big!");
			VK_DescriptorInfo uniform_info = generateDescriptorInfo(vk_stage, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, glsl, sb, limits);
			uniforms.emplace_back(uniform_info);
		}
		//--------------------------------------------------------IMAGE----------------------------------------
		for (auto &image: resources.separate_images) {
			if (used_variables.find(image.id) == used_variables.end()) {
				//continue;
			}
			VK_DescriptorInfo uniform_info = generateDescriptorInfo(vk_stage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, glsl, image, limits);
			uniforms.emplace_back(uniform_info);
		}
		//----------------------------------------------STORAGE_IMAGES & TEXEL BUFFERS------------------------------------------------
		for (auto &image: resources.storage_images) {
			VK_DescriptorInfo uniform_info;
			auto type = glsl.get_type(image.type_id);
			if (type.image.dim == spv::DimBuffer) {
				// The image is an imageBuffer
				uniform_info = generateDescriptorInfo(vk_stage, VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, glsl, image, limits);
			} else {
				// The image is a storageImage
				uniform_info = generateDescriptorInfo(vk_stage, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, glsl, image, limits);
			}
			uniforms.emplace_back(uniform_info);
		}
		//----------------------------------------------------------ACCELERATION_STRUCTURES----------------------------------------------------------
		for (auto &as: resources.acceleration_structures) {
			VK_DescriptorInfo uniform_info = generateDescriptorInfo(vk_stage, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, glsl, as, limits);
			uniforms.emplace_back(uniform_info);
		}
		//----------------------------------------------PUSH CONSTANTS------------------------------------------------
		for (auto &pc: resources.push_constant_buffers) {
			size_t size = glsl.get_declared_struct_size(glsl.get_type(pc.base_type_id));
			HB_ASSERT(size <= device->getPhysicalDevice().getProperties().limits.maxPushConstantsSize, "Push constant size is too big!");
			std::string name = glsl.get_name(pc.id);

			VK_PushConstantInfo push_constant_info{};
			push_constant_info.name = name;

			VkPushConstantRange push_constant_range{};
			push_constant_range.size = size;
			push_constant_range.offset = glsl.get_decoration(pc.base_type_id, spv::DecorationOffset);
			push_constant_range.stageFlags = vk_stage;

			push_constant_info.push_constant_range = push_constant_range;
			push_constants.emplace_back(push_constant_info);
		}
		std::sort(push_constants.begin(), push_constants.end(),
		          [](const VK_PushConstantInfo &a, const VK_PushConstantInfo &b) -> bool {
			          return a.push_constant_range.offset < b.push_constant_range.offset;
		          });

		//----------------------------------------------------------VERTEX INPUTS-----------------------------
		for (auto &stage_input: resources.stage_inputs) {
			spirv_cross::SPIRType type = glsl.get_type(stage_input.base_type_id);

			uint32_t location = glsl.get_decoration(stage_input.id, spv::DecorationLocation);
			for (size_t j = 0; j < type.columns; ++j) {
				VK_VertexAttributeInfo attribute_description{};
				attribute_description.location = location;
				attribute_description.size = type.vecsize * 4;

				switch (type.vecsize) {
					case 1:
						if (type.basetype == spirv_cross::SPIRType::BaseType::Float)
							attribute_description.format = VK_FORMAT_R32_SFLOAT;
						else if (type.basetype == spirv_cross::SPIRType::BaseType::UInt)
							attribute_description.format = VK_FORMAT_R32_UINT;
						break;
					case 2:
						if (type.basetype == spirv_cross::SPIRType::BaseType::Float)
							attribute_description.format = VK_FORMAT_R32G32_SFLOAT;
						else if (type.basetype == spirv_cross::SPIRType::BaseType::UInt)
							attribute_description.format = VK_FORMAT_R32G32_UINT;
						break;
					case 3:
						if (type.basetype == spirv_cross::SPIRType::BaseType::Float)
							attribute_description.format = VK_FORMAT_R32G32B32_SFLOAT;
						else if (type.basetype == spirv_cross::SPIRType::BaseType::UInt)
							attribute_description.format = VK_FORMAT_R32G32B32_UINT;
						break;
					case 4:
						if (type.basetype == spirv_cross::SPIRType::BaseType::Float)
							attribute_description.format = VK_FORMAT_R32G32B32A32_SFLOAT;
						else if (type.basetype == spirv_cross::SPIRType::BaseType::UInt)
							attribute_description.format = VK_FORMAT_R32G32B32A32_UINT;
						break;
				}
				vertex_inputs.emplace_back(attribute_description);
				location++;
			}
		}
		if (this->stage == SHADER_STAGE_COMPUTE) {
			// Get the entry point.
			auto entry_point = glsl.get_entry_point(info.preamble, spv::ExecutionModelGLCompute);
			// Get the workgroup size.
			workgroup_size.x = entry_point.workgroup_size.x;
			workgroup_size.y = entry_point.workgroup_size.y;
			workgroup_size.z = entry_point.workgroup_size.z;
		}

		std::sort(uniforms.begin(), uniforms.end(),
		          [](const VK_DescriptorInfo &a, const VK_DescriptorInfo &b) -> bool {
			          return a.layout_binding.binding < b.layout_binding.binding;
		          });
		std::sort(vertex_inputs.begin(), vertex_inputs.end(),
		          [](const VK_VertexAttributeInfo &a, const VK_VertexAttributeInfo &b) -> bool {
			          return a.location < b.location;
		          });
	}


	VkShaderStageFlagBits VK_Shader::getVkStage() const {
		return vk_stage;
	}

	const std::vector<VK_PushConstantInfo> &VK_Shader::getPushConstants() const {
		return push_constants;
	}

	const std::vector<VK_DescriptorInfo> &VK_Shader::getDescriptorInfos() const {
		return uniforms;
	}

	const std::vector<VK_VertexAttributeInfo> &VK_Shader::getVertexInputs() const {
		return vertex_inputs;
	}

}
