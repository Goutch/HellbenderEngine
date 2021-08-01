//
// Created by User on 2021-08-01.
//

#include <vector>
#include "VK_DescriptorPool.h"
#include "VK_Device.h"
#include "core/utility/Log.h"

HBE::VK_DescriptorPool::VK_DescriptorPool(VK_Device *device, std::vector<VkDescriptorSetLayout>& layouts, uint32_t descriptor_sets_count) {
    this->device = device;
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = descriptor_sets_count;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;

    poolInfo.maxSets = descriptor_sets_count;

    if (vkCreateDescriptorPool(device->getHandle(), &poolInfo, nullptr, &handle) != VK_SUCCESS) {
        Log::error("failed to create descriptor pool!");
    }
    createDescriptorSets(descriptor_sets_count);
}

void HBE::VK_DescriptorPool::createDescriptorSets( std::vector<VkDescriptorSetLayout>& layouts) {

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = handle;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
    allocInfo.pSetLayouts = layouts.data();

    descriptor_sets.resize(layouts.size());
    if (vkAllocateDescriptorSets(device->getHandle(), &allocInfo, descriptor_sets.data()) != VK_SUCCESS) {
        Log::error("failed to allocate descriptor sets!");
    }
}
