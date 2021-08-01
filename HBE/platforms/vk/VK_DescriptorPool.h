#pragma once

#include "vulkan/vulkan.h"

namespace HBE {
    class VK_Device;

    class VK_DescriptorPool {


        VK_Device *device;
        VkDescriptorPool handle;
        std::vector<VkDescriptorSet> descriptor_sets;
    public:
        ~VK_DescriptorPool();

        VK_DescriptorPool(VK_Device *device, std::vector<VkDescriptorSetLayout> &layouts, uint32_t descriptor_sets_count);

    private:

        void createDescriptorSets(std::vector<VkDescriptorSetLayout> &layouts);
    };
}

