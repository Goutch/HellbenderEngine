#pragma once
#include "VK_Device.h"
namespace HBE{
	class VK_Scheduler {
		uint32_t current_operation=0;
		VK_Device* device;
		std::vector<VK_Fence> fences;
		VK_Scheduler(VK_Device* device,uint32_t concurrent_operations_count);
		void wait();
		VK_Fence* getFence();
	};
}

