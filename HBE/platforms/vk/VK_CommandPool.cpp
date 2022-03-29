
#include "VK_CommandPool.h"
#include "core/utility/Log.h"
#include "VK_Device.h"
#include "VK_Swapchain.h"

namespace HBE {
	HBE::VK_CommandPool::~VK_CommandPool() {
		for (VK_Fence *fence:fences) {
			fence->wait();
			delete fence;
		}
		fences.clear();
		vkDestroyCommandPool(device.getHandle(), handle, nullptr);
	}

	void HBE::VK_CommandPool::clear() {
		for (VK_Fence *fence:fences) {
			fence->wait();
			delete fence;
		}
		fences.clear();
		vkFreeCommandBuffers(device.getHandle(), handle, static_cast<uint32_t>(command_buffers.size()),
							 command_buffers.data());
		command_buffers.clear();
	}

	VK_CommandPool::VK_CommandPool(VK_Device &device, int command_buffers_count) : device(device) {
		QueueFamilyIndices queueFamilyIndices = device.getPhysicalDevice().getQueueFamilyIndices();

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphics_family.value();

		poolInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
						 VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;


		if (vkCreateCommandPool(device.getHandle(), &poolInfo, nullptr, &handle) != VK_SUCCESS) {
			Log::error("failed to create command pool!");
		}
		createCommandBuffers(command_buffers_count);
	}

	void HBE::VK_CommandPool::createCommandBuffers(int count) {
		current = count - 1;
		for (int i = 0; i < count; ++i) {
			fences.emplace_back(new VK_Fence(device));
		}
		command_buffers.resize(count);
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = handle;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t) command_buffers.size();

		if (vkAllocateCommandBuffers(device.getHandle(), &allocInfo, command_buffers.data()) != VK_SUCCESS) {
			Log::error("failed to allocate command buffers!");
		}
	}

	void VK_CommandPool::begin() const {
		begin(current);
	}

	void VK_CommandPool::end() const {
		end(current);
	}

	void HBE::VK_CommandPool::begin(uint32_t i) const {
		current = i;

		fences[current]->wait();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(command_buffers[i], &beginInfo) != VK_SUCCESS) {
			Log::error("failed to begin recording command buffer!");
		}
	}

	void HBE::VK_CommandPool::end(uint32_t i) const {
		if (vkEndCommandBuffer(command_buffers[i]) != VK_SUCCESS) {
			Log::error("failed to record command buffer!");
		}
	}

	VK_Fence &VK_CommandPool::getCurrentFence() {
		return *fences[current];
	}

	const std::vector<VkCommandBuffer> &HBE::VK_CommandPool::getBuffers() const {
		return command_buffers;
	}


	void VK_CommandPool::reset(uint32_t i) {
		vkResetCommandBuffer(command_buffers[i], 0);
	}

	const VkCommandBuffer &VK_CommandPool::getCurrentBuffer() const {
		return command_buffers[current];
	}

	const VkCommandPool &VK_CommandPool::getHandle() const {
		return handle;
	}

	void VK_CommandPool::submit(HBE::QUEUE_FAMILY queue, VkSemaphore *wait, VkPipelineStageFlags *wait_stage, uint32_t wait_count, VkSemaphore *signal, uint32_t signal_count) {
		fences[current]->reset();
		device.getQueue(queue)->submit(command_buffers[current], fences[current]->getHandle(), wait, wait_stage, wait_count, signal, signal_count);
		current++;
		current %= command_buffers.size();
	}


}

