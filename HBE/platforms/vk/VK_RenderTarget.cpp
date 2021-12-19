#include "VK_RenderTarget.h"

namespace HBE{
	/*VK_RenderTarget::VK_RenderTarget(uint32_t width,uint32_t height) : RenderTarget(width,height, nullptr){
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = (float) height;
		viewport.width = (float) width;
		viewport.height = (float) -height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
	}

	VkViewport &VK_RenderTarget::getViewport() {
		return viewport;
	}

	void VK_RenderTarget::begin(VkCommandBuffer command_buffer) {
		vkCmdSetViewport(command_buffer,0,1,&viewport);
	}*/
}
