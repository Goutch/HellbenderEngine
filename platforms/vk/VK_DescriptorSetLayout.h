#pragma once

#include "vulkan/vulkan.h"
#include "vector"

namespace HBE
{
    class VK_Context;
    class VK_Device;

    struct VK_DescriptorInfo;

    class VK_DescriptorSetLayout
    {
        VkDescriptorSetLayout handle = VK_NULL_HANDLE;
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        std::vector<VK_DescriptorInfo> descriptor_infos;
        VK_Context* context = nullptr;

    public :
        void init(VK_Context* context, uint32_t descriptor_set_index, std::vector<VK_DescriptorInfo>& pipeline_descriptors, bool empty_descriptor_allowed);
        void release();
        VK_DescriptorSetLayout() = default;
        VK_DescriptorSetLayout(VK_DescriptorSetLayout&& other) noexcept;
        ~VK_DescriptorSetLayout() = default;
        VK_DescriptorSetLayout(const VK_DescriptorSetLayout&) = delete;
        VK_DescriptorSetLayout& operator=(const VK_DescriptorSetLayout&) = delete;


        VkDescriptorSetLayout getHandle() const;

        const std::vector<VkDescriptorSetLayoutBinding>& getBindings() const;

        const std::vector<VK_DescriptorInfo>& getDescriptorInfos() const;

        uint32_t getLastBinding() const;
    };
}
