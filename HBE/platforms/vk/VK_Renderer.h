#pragma once

#include "core/graphics/Renderer.h"
#include "vulkan/vulkan.h"
#include "VK_Semaphore.h"
#include "unordered_map"
#include "VK_CONSTANTS.h"
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
#define MAP(T1, T2) std::unordered_map<T1,T2>
	private:
		struct FrameState {
			VK_Semaphore *finished_semaphore;
			VK_Semaphore *image_available_semaphore;
		};

		MAP(const GraphicPipeline*, MAP(GraphicPipelineInstance * , MAP(const Mesh*, std::vector<std::vector<PushConstantInfo>>))) render_cache;

		std::vector<DrawCmdInfo> ordered_render_cache;

		const size_t PUSH_CONSTANT_BLOCK_SIZE = 1024 * 16; //16kb
		size_t current_pc_block = 0;
		size_t current_pc_block_offset = 0;
		std::vector<char *> push_constant_blocks;

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
		GraphicPipeline *screen_pipeline = nullptr;
		GraphicPipelineInstance *screen_material = nullptr;
		bool windowResized = false;
		bool frame_presented = false;


	public:
		Event<uint32_t> onFrameChange;

		void render(const RenderTarget *render_target,
					const mat4 &projection_matrix,
					const mat4 &view_matrix) override;

		void raytrace(const RootAccelerationStructure &root_acceleration_structure,
					  RaytracingPipelineInstance &pipeline,
					  const mat4 &projection_matrix,
					  const mat4 &view_matrix,
					  const vec2i resolution) override;


		void waitCurrentFrame();
		RenderTarget *getDefaultRenderTarget() override;

		void beginFrame() override;

		void present(const Texture *image) override;

		void endFrame() override;

		void draw(DrawCmdInfo &draw_cmd_info) override;

		VK_Renderer();

		~VK_Renderer() override;

		const ResourceFactory *getResourceFactory() const override;

		VK_CommandPool *getCommandPool();

		VK_Device *getDevice();
		uint32_t getFrameCount() const override;

		void onWindowClosed();

		void onWindowSizeChange(uint32_t width, uint32_t height);

		void reCreateSwapchain();

		const VK_Swapchain &getSwapchain() const;

		uint32_t getCurrentFrame() const override;

		void createDefaultResources() override;

		const VK_Instance *getInstance() const;
		void waitAll();
		VkSampler getDefaultSampler();
	};
}


