#pragma once

#include <array>
#include <map>
#include "platforms/vk/VK_DescriptorSetLayout.h"
#include "vector"
#include "vulkan/vulkan.h"
#include "VK_Shader.h"

namespace HBE
{
    class VK_Shader;

    class VK_Device;

    class RasterizationTarget;

    class Image;

    class VK_DescriptorSetLayout;

    class VK_PipelineLayout
    {
        VkPipelineLayout handle = VK_NULL_HANDLE;
        VK_Context* context = nullptr;

		//vulkan flat handles
        std::vector<VkDescriptorSetLayout> descriptor_set_layout_handles;
	    std::vector<VkDescriptorSetLayoutBinding> layout_binding_handles;
	    std::vector<VkPushConstantRange> push_constants_ranges;

		//HBE Vulkan data transfer objects
        std::vector<VK_BindingInfo> pipeline_bindings;
	    std::vector<VK_DescriptorSetLayout> descriptor_set_layouts;
        std::vector<VK_PushConstantInfo> pipeline_push_constants;

		//maps
	    std::map<std::string, size_t> descriptor_name_to_binding;
        std::map<std::string, size_t> push_constant_name_to_index;


        VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;

    public:
        void init(VK_Context* context, ShaderHandle* shaders, size_t count, bool empty_descriptor_allowed = false);
        void release();
        VK_PipelineLayout() = default;
        VK_PipelineLayout(VK_PipelineLayout&& other) noexcept;
        ~VK_PipelineLayout() = default;

        VkPipelineLayout getHandle() const;

        void pushConstant(VkCommandBuffer command_buffer, const char* name, const void* data) const;

        const std::vector<VkDescriptorSetLayoutBinding>& getDescriptorBindings() const;

        uint32_t getDescriptorBinding(const char* name) const;

        VkPipelineBindPoint getBindPoint() const;

        const std::vector<VkDescriptorSetLayout>& getDescriptorSetLayoutHandles() const;

		const std::vector<VK_DescriptorSetLayout>& getDescriptorSetLayouts() const;

        const std::vector<VK_BindingInfo>& getBindingInfos() const;

        bool IsBindingVariableSize(uint32_t binding) const;

        void mergeStages(ShaderHandle* shaders, size_t count);

        void mergeBindingInfoStages(VK_BindingInfo& merged_binding_info, VK_BindingInfo& old_binding_info, VK_BindingInfo& new_binding_info);

        uint32_t getLastDescriptorSetBinding(uint32_t set) const;

		VkDeviceSize getBindingElementSize(uint32_t binding) const;

        uint32_t getDescriptorSetCount() const;
    };
}
