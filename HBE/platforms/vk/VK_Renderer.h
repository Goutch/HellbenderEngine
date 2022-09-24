#pragma once

#include "core/graphics/Renderer.h"
#include "vulkan/vulkan.h"
#include "VK_Semaphore.h"
#include "unordered_map"
#include "VK_CONSTANTS.h"
#include "list"
#include "array"

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

		MAP(const GraphicPipeline*, MAP(Material * , MAP(const Mesh*, std::vector<mat4>))) render_cache;
		MAP(const GraphicPipeline*, MAP(Material * , MAP(const Mesh*, std::vector<mat3>))) render_cache_2D;
		MAP(const GraphicPipeline*, MAP(Material * , std::vector<const Mesh *>)) instanced_cache;

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
		Material *screen_material = nullptr;
		bool windowResized = false;
		bool frame_presented = false;
	public:
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

		void present(Texture *image) override;

		void endFrame() override;

		void draw(mat4 transform_matrix, const Mesh &mesh, Material &material) override;

		void drawInstanced(const Mesh &mesh, Material &material) override;

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


