#pragma once

#include "core/graphics/Renderer.h"
#include "vulkan/vulkan.h"
#include "VK_Semaphore.h"
#include "unordered_map"
#include "VK_CONSTANTS.h"
#include "list"
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
#define MAP_LIST(T1, T2) std::unordered_map<T1,T2>
	private:
		struct RenderObject {
			const Transform *transform;
			const Mesh *mesh;
			const Material *material;
		};
		MAP_LIST(const Material*, MAP_LIST(const Mesh*, std::list<const Transform*>)) render_cache;


		VK_Window *window;
		VK_Instance *instance;
		VK_Surface *surface;
		VK_PhysicalDevice *physical_device;
		VK_Device *device;
		VK_ResourceFactory *factory;
		VK_Swapchain *swapchain;
		VK_CommandPool *command_pool;
		uint32_t current_frame = 0;
		uint32_t current_image = 0;
		std::vector<VK_Semaphore *> image_available_semaphores;
		std::vector<VK_Semaphore *> render_finished_semaphores;
		std::vector<VK_Fence *> frames_in_flight_fences;
		std::vector<VK_Fence *> images_in_flight_fences;
		bool windowResized = false;
	public:
		void render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix) override;

		void present(const RenderTarget *render_target) override;

		void beginFrame() override;

		void endFrame() override;

		void draw(const Transform &transform, const Mesh &mesh, const Material &material) override;

		void drawInstanced(const Mesh &mesh, const Material &material) override;

		void clear() const override;

		VK_Renderer();

		~VK_Renderer();

		const ResourceFactory *getResourceFactory() const override;

		VK_CommandPool *getCommandPool();

		VK_Device *getDevice();
		void onWindowClosed();

		void onWindowSizeChange(int width, int height);

		void reCreateSwapchain();

		const VK_Swapchain &getSwapchain() const;

		uint32_t getCurrentFrame() const;
	};
}


