#pragma once

#include <array>
#include "vector"
#include "vulkan/vulkan.h"
#include "VK_Shader.h"
#include "VK_DescriptorSetLayout.h"
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
        std::vector<VkDescriptorSetLayout> descriptor_set_layout_handles;
        std::vector<VK_DescriptorSetLayout> descriptor_set_layouts;
        std::map<std::string, size_t> descriptor_name_to_binding;
        std::vector<bool> variable_descriptors;
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        std::vector<VkPushConstantRange> push_constants_ranges;
        std::vector<VkDeviceSize> descriptor_sizes;
        std::vector<VK_DescriptorInfo> pipeline_descriptors;
        std::vector<VK_PushConstantInfo> pipeline_push_constants;
        std::map<std::string, size_t> push_constant_name_to_index;


        VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;

    public:
        void init(VK_Context* context, const VK_Shader** shaders, size_t count, bool empty_descriptor_allowed = false);
        void release();
        VK_PipelineLayout() = default;
        VK_PipelineLayout(VK_PipelineLayout&& other) noexcept;
        ~VK_PipelineLayout() = default;

        VkPipelineLayout getHandle() const;

        void pushConstant(VkCommandBuffer command_buffer, const std::string& name, const void* data) const;

        const std::vector<VkDescriptorSetLayoutBinding>& getDescriptorBindings() const;

        uint32_t getDescriptorBinding(const std::string& name) const;

        VkPipelineBindPoint getBindPoint() const;

        const std::vector<VkDeviceSize>& getDescriptorSizes() const;

        const std::vector<VkDescriptorSetLayout>& getDescriptorSetLayoutHandles() const;

        const std::vector<VK_DescriptorInfo>& getDescriptorInfos() const;

        bool IsBindingVariableSize(uint32_t binding) const;

        void mergeStages(const VK_Shader** shaders, size_t count);

        void mergeDescriptorStages(VK_DescriptorInfo& merged_descriptor, VK_DescriptorInfo& old_descriptor, VK_DescriptorInfo& new_descriptor);

        uint32_t getLastDescriptorSetBinding(uint32_t set) const;

        uint32_t getDescriptorSetCount() const;
    };
}
