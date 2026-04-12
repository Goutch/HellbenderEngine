#pragma once
#include "Core.h"
#include "core/interface/RendererInterface.h"
#include "vulkan/vulkan.h"
#include "HBE/platforms/vk/resources/VK_Semaphore.h"
#include "array"
#include "VK_CommandPool.h"
#include "core/graphics/GraphicLimits.h"
#include "dependencies/utils-collection/Event.h"
#include "resources/VK_RasterizationPipeline.h"

namespace HBE {
	class Window;

	class VK_Window;

	class VK_Instance;

	class VK_PhysicalDevice;

	class VK_Device;

	class VK_Surface;

	class VK_Swapchain;

	class VK_RenderPass;

	class VK_CommandPool;

	class VK_Fence;

	class HB_API VK_Renderer {
		struct SwapchainImageState {
			VK_Semaphore finished_semaphore{};
			VK_Semaphore image_available_semaphore{};
		};

		std::vector<SwapchainImageState> swap_chain_image_state;

		VkSampler default_sampler;

		bool windowResized = false;
		bool frame_presented = false;

		event_subscription_id vertical_sync_changed_subscription_id;
		event_subscription_id window_closed_subscription_id;
		event_subscription_id window_size_changed_subscription_id;

		VK_CommandPool command_pool;
		VK_Context *context = nullptr;

		RendererResources renderer_resources;
	public:
		Event<uint32_t> onFrameEnd;

		void init(VK_Context *context);

		void release();

		VK_Renderer() = default;

		~VK_Renderer() = default;

		VK_Renderer(const VK_Renderer &) = delete;

		VK_Renderer &operator=(const VK_Renderer &) = delete;

		void cmdRasterizeGraph(const RasterizeGraphCmdInfo &info);

		void cmdTraceRays(const TraceRaysCmdInfo &trace_rays_cmd_info);

		void cmdPresent(const PresentCmdInfo &present_cmd_info);

		void cmdDispatch(const ComputeDispatchCmdInfo &compute_dispatch_cmd_info);

		void cmdDispatchAsync(const ComputeDispatchCmdInfo &compute_dispatch_cmd_info);
		void getRendererResrouces(RendererResources& resources);

		void beginFrame();

		void endFrame();

		VK_CommandPool *getCommandPool();

		uint32_t getFrameCount() const;

		void onWindowClosed();

		void onWindowSizeChange(Window *window);

		void reCreateSwapChain();

		uint32_t getCurrentFrameIndex() const;

		void waitAll();

		VkSampler getDefaultSampler();

		GraphicLimits getLimits();

		FenceHandle getLastFrameFence();

		FenceHandle getCurrentFrameFence();

	private:
		void createDefaultResources();
	};
}
