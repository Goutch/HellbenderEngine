#pragma once

#include "core/graphics/Renderer.h"
#include "vulkan/vulkan.h"
#include "VK_Semaphore.h"

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
        const unsigned int MAX_FRAMES_IN_FLIGHT = 2;

        VK_Window *window;
        VK_Instance *instance;
        VK_Surface *surface;
        VK_PhysicalDevice* physical_device;
        VK_Device* device;
        VK_ResourceFactory* factory;
        VK_Swapchain* swapchain;
        VK_RenderPass* render_pass;
        VK_CommandPool* command_pool;

        size_t current_frame = 0;
        std::vector<VK_Semaphore*> image_available_semaphores;
        std::vector<VK_Semaphore*> render_finished_semaphores;
        std::vector<VK_Fence*> in_flight_fences;
        std::vector<VK_Fence*> images_in_flight_fences;
        const VkCommandBuffer* current_command_buffer= nullptr;
    public:
        void render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix) override;

        void present(const RenderTarget *render_target) override;

        void clearDrawCache() override;

        void draw(const Transform &transform, const Mesh &mesh, const Material &material) override;

        void drawInstanced(const Mesh &mesh, const Material &material) override;

        void clear() const override;

        VK_Renderer();
        ~VK_Renderer();

        const IResourceFactory *getResourceFactory() const override;

        const VkCommandBuffer* getCurrentCommandBuffer() const ;

        const VK_RenderPass& getRenderPass() const;

        void onApplicationQuit();

        void onWindowClosed();
    };
}


