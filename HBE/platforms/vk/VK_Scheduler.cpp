
#include "VK_Scheduler.h"
#include "VK_Fence.h"

namespace HBE {
	VK_Scheduler::VK_Scheduler(VK_Device *device, uint32_t concurrent_operations_count) {
		this->device = device;
		for (size_t i = 0; i < concurrent_operations_count; ++i) {
			fences.emplace_back(*device);
		}
	}

	void VK_Scheduler::wait() {
		for (uint32_t i = 0; i < fences.size(); ++i) {
			fences[i].wait();
			fences[i].reset();
		}
	}

	VK_Fence *VK_Scheduler::getFence() {
		VK_Fence* fence=&fences[current_operation];
		fence->wait();
		fence->reset();
		current_operation = (current_operation+1) % fences.size();
		return fence;
	}
}