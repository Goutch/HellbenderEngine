#pragma once

#include "core/graphics/Renderer.h"
#include "vulkan/vulkan.hpp"

namespace HBE {
    class VK_Window;
    class VK_Instance;
    class VK_PhysicalDevice;
    class VK_Device;
    class VK_Surface;
    class VK_Swapchain;
    class VK_ResourceFactory;
    class VK_Renderer : public Renderer {
        VK_Window *window;
        VK_Instance *instance;
        VK_Surface *surface;
        VK_PhysicalDevice* physical_device;
        VK_Device* device;
        VK_ResourceFactory* factory;
        VK_Swapchain* swapchain;
    public:
        void render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix) override;

        void present(const RenderTarget *render_target) override;

        void clearDrawCache() override;

        void draw(const Transform &transform, const Mesh &mesh, const Material &material, DRAW_FLAGS flags) override;

        void drawInstanced(const Mesh &mesh, const Material &material, DRAW_FLAGS drawFlags) override;

        void clear() const override;

        VK_Renderer();
        ~VK_Renderer();

        const IResourceFactory *getResourceFactory() override;
    };
}


