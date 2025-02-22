#pragma once

#include "core/resource/raytracing/RaytracingPipeline.h"
#include "vulkan/vulkan.h"

namespace HBE
{
	class VK_Renderer;

	class VK_Buffer;

	class VK_Device;

	class VK_PipelineLayout;

	class VK_Shader;


	class VK_Renderer;

	class VK_RaytracingPipeline : public RaytracingPipeline
	{
		RaytracingPipelineInfo info;
		VkPipeline handle;
		VK_Device* device = nullptr;
		VK_Renderer* renderer;
		VK_PipelineLayout* pipeline_layout = nullptr;

		VK_Buffer* shader_binding_table_buffer = nullptr;

		VK_Shader** any_hit_shaders;
		uint32_t any_hit_shader_count;

		VK_Shader** intersection_shaders;
		uint32_t intersection_shader_count;

		std::vector<VkRayTracingShaderGroupCreateInfoKHR> shader_groups{};

		VK_Buffer* raygen_shader_binding_table_buffer = nullptr;
		VK_Buffer* miss_shader_binding_table_buffer = nullptr;
		VK_Buffer* hit_shader_binding_table_buffer = nullptr;

		VkStridedDeviceAddressRegionKHR raygen_shader_binding_table_address{};
		VkStridedDeviceAddressRegionKHR miss_shader_binding_table_address{};
		VkStridedDeviceAddressRegionKHR hit_shader_binding_table_address{};
		VkStridedDeviceAddressRegionKHR callable_shader_binding_table_address{};
		mutable bool is_bound = false;

	public:
		VK_RaytracingPipeline(VK_Renderer* renderer, const RaytracingPipelineInfo& info);

		~VK_RaytracingPipeline() override;

		void bind() const;

		void unbind() const;

		RAYTRACING_PIPELINE_FLAGS getFlags() const override;

		const VkStridedDeviceAddressRegionKHR& getRaygenShaderBindingTable() const;

		const VkStridedDeviceAddressRegionKHR& getMissShaderBindingTable() const;

		const VkStridedDeviceAddressRegionKHR& getHitShaderBindingTable() const;

		const VkStridedDeviceAddressRegionKHR& getCallableShaderBindingTable() const;

		const VK_PipelineLayout* getPipelineLayout() const;

	private:
		void createShaderBindingTable(const RaytracingPipelineInfo& info);
	};
}
