#pragma once

#include "vulkan/vulkan.h"
#include "vector"
#include "VK_DescriptorPoolSize.h"

namespace HBE
{
    class VK_Context;
    class VK_Device;

    struct VK_BindingInfo;

    class VK_DescriptorSetLayout
    {
        VkDescriptorSetLayout handle = VK_NULL_HANDLE;
        std::vector<VkDescriptorSetLayoutBinding> layout_bindings;
        std::vector<VK_BindingInfo> set_bindings;
        VK_Context* context = nullptr;
		uint32_t descriptor_set_id = 0;
	    VK_DescriptorPoolSize required_pool_sizes;
    public :
        void init(VK_Context* context, uint32_t descriptor_set_index, std::vector<VK_BindingInfo>& pipeline_bindings, bool empty_descriptor_allowed);
        void release();
        VK_DescriptorSetLayout() = default;
        VK_DescriptorSetLayout(VK_DescriptorSetLayout&& other) noexcept;
        ~VK_DescriptorSetLayout() = default;
        VK_DescriptorSetLayout(const VK_DescriptorSetLayout&) = delete;
        VK_DescriptorSetLayout& operator=(const VK_DescriptorSetLayout&) = delete;

		uint32_t getDescriptorSetId() const;
        VkDescriptorSetLayout getHandle() const;

        const std::vector<VkDescriptorSetLayoutBinding>& getLayoutBindings() const;

        const std::vector<VK_BindingInfo>& getBindingInfos() const;

		VkDescriptorType getLastBindingType() const;

        uint32_t getLastBinding() const;

	    const VK_DescriptorPoolSize& getRequiredPoolSizes() const;
    };
}
