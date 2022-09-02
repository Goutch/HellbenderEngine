//
// Created by user on 8/30/2022.
//

#include "VK_RaytracingPipeline.h"
#include "vulkan/vulkan.h"
#include "../VK_Renderer.h"
#include "../VK_PipelineLayout.h"

namespace HBE {
	VK_RaytracingPipeline::VK_RaytracingPipeline(HBE::VK_Renderer *renderer, const HBE::RaytracingPipelineInfo &info) {
		this->device = renderer->getDevice();

		VK_Shader *raygen_shader = dynamic_cast<VK_Shader *>(info.raygen_shader);
		VK_Shader *miss_shader = dynamic_cast<VK_Shader *>(info.miss_shader);
		VK_Shader *closest_hit_shader = dynamic_cast<VK_Shader *>(info.closest_hit_shader);
		const VK_Shader *shaders[] = {
				raygen_shader,
				miss_shader,
				closest_hit_shader,
		};

		layout = new VK_PipelineLayout(device, shaders, 2);

		/*
			Setup ray tracing shader groups
		*/
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;


		std::vector<VkRayTracingShaderGroupCreateInfoKHR> shaderGroups{};
		// Ray generation group
		{
			shaderStages.push_back(loadShader(getShadersPath() + "raytracingbasic/raygen.rgen.spv", VK_SHADER_STAGE_RAYGEN_BIT_KHR));
			VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
			shaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
			shaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
			shaderGroup.generalShader = static_cast<uint32_t>(shaderStages.size()) - 1;
			shaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
			shaderGroups.push_back(shaderGroup);
		}

		// Miss group
		{
			shaderStages.push_back(loadShader(getShadersPath() + "raytracingbasic/miss.rmiss.spv", VK_SHADER_STAGE_MISS_BIT_KHR));
			VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
			shaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
			shaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
			shaderGroup.generalShader = static_cast<uint32_t>(shaderStages.size()) - 1;
			shaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
			shaderGroups.push_back(shaderGroup);
		}

		// Closest hit group
		{
			shaderStages.push_back(loadShader(getShadersPath() + "raytracingbasic/closesthit.rchit.spv", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR));
			VkRayTracingShaderGroupCreateInfoKHR shaderGroup{};
			shaderGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
			shaderGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
			shaderGroup.generalShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.closestHitShader = static_cast<uint32_t>(shaderStages.size()) - 1;
			shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
			shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
			shaderGroups.push_back(shaderGroup);
		}

		/*
			Create the ray tracing pipeline
		*/
		VkRayTracingPipelineCreateInfoKHR rayTracingPipelineInfo{};
		rayTracingPipelineInfo.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
		rayTracingPipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		rayTracingPipelineInfo.pStages = shaderStages.data();
		rayTracingPipelineInfo.groupCount = static_cast<uint32_t>(shaderGroups.size());
		rayTracingPipelineInfo.pGroups = shaderGroups.data();
		rayTracingPipelineInfo.maxPipelineRayRecursionDepth = 1;
		rayTracingPipelineInfo.layout = pipelineLayout;
		VK_CHECK_RESULT(vkCreateRayTracingPipelinesKHR(renderer., VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &rayTracingPipelineCI, nullptr, &handle));
	}

}
