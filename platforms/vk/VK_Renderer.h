#pragma once

#include "core/graphics/Renderer.h"
#include "vulkan/vulkan.h"
#include "VK_Semaphore.h"
#include "unordered_map"
#include "list"
#include "array"
#include "core/utility/Event.h"

namespace HBE {
	class VK_Window;

	class VK_Instance;

	class VK_PhysicalDevice;

	class VK_Device;

	class VK_Surface;

	class VK_Swapchain;

	class VK_ResourceFactory;

	class VK_RenderPass;

	class VK_CommandPool;

	class VK_Fence;


	class VK_Renderer : public Renderer {

	private:
		struct FrameState {
			VK_Semaphore *finished_semaphore;
			VK_Semaphore *image_available_semaphore;
		};

		VK_Window *window;
		VK_Instance *instance;
		VK_Surface *surface;
		VK_PhysicalDevice *physical_device;
		VK_Device *device;
		VK_ResourceFactory *factory;
		VK_Swapchain *swapchain;
		VK_CommandPool *command_pool;

		VkSampler default_sampler;

		uint32_t current_frame = 0;
		uint32_t current_image = 0;
		std::array<FrameState, MAX_FRAMES_IN_FLIGHT> frames;
		std::vector<VK_Fence *> images_in_flight_fences;

		RenderTarget *main_render_target = nullptr;
		RenderTarget *ui_render_target = nullptr;
		GraphicPipeline *screen_pipeline = nullptr;
		GraphicPipelineInstance *screen_pipeline_instance = nullptr;
		bool windowResized = false;
		bool frame_presented = false;


	public:
		void render(RenderCmdInfo &render_cmd_info) override;

		void traceRays(TraceRaysCmdInfo &trace_rays_cmd_info) override;

		void present(PresentCmdInfo &present_cmd_info) override;

		void waitCurrentFrame() override;
		void waitLastFrame() override;

		RenderTarget *getDefaultRenderTarget() override;

		RenderTarget *getUIRenderTarget() override;

		void beginFrame() override;

		void endFrame() override;

		VK_Renderer();

		~VK_Renderer() override;

		const ResourceFactory *getResourceFactory() const override;

		VK_CommandPool *getCommandPool();

		VK_Device *getDevice();

		uint32_t getFrameCount() const override;

		void onWindowClosed();

		void onWindowSizeChange(Window *window);

		void reCreateSwapchain();

		const VK_Swapchain &getSwapchain() const;

		uint32_t getCurrentFrame() const override;


		const VK_Instance *getInstance() const;

		void waitAll();

		VkSampler getDefaultSampler();

	private:

		void createDefaultResources() override;
	};
}


