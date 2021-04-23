

#include "VK_Renderer.h"

#define GLFW_INCLUDE_VULKAN

#include "Configs.h"
#include "VK_Window.h"
#include "VK_Instance.h"
#include "VK_PhysicalDevice.h"
#include "VK_Swapchain.h"
#include "VK_Device.h"
#include "VK_Surface.h"
#include "VK_Swapchain.h"
#include "VK_ResourceFactory.h"

namespace HBE {
    VK_Renderer::VK_Renderer() {
        window = dynamic_cast<VK_Window *>(Graphics::getWindow());
        int width, height;
        window->getSize(width, height);

        instance = new VK_Instance();
        surface = new VK_Surface(instance->getHandle(), window->getHandle());
        physical_device = new VK_PhysicalDevice(instance->getHandle(), surface->getHandle());
        device = new VK_Device(*physical_device);
        swapchain = new VK_Swapchain(width, height, surface->getHandle(), *device);
        factory = new VK_ResourceFactory(device);
    }


    VK_Renderer::~VK_Renderer() {
        delete factory;
        delete swapchain;
        delete device;
        delete physical_device;
        delete surface;
        delete instance;
    }

    const IResourceFactory *VK_Renderer::getResourceFactory() const {
        return factory;
    }

    void VK_Renderer::render(const HBE::RenderTarget *render_target,
                             const mat4 &projection_matrix,
                             const mat4 &view_matrix) {

    }

    void VK_Renderer::present(const HBE::RenderTarget *render_target) {

    }

    void VK_Renderer::clearDrawCache() {

    }

    void VK_Renderer::draw(const HBE::Transform &transform,
                           const HBE::Mesh &mesh,
                           const HBE::Material &material) {

    }

    void VK_Renderer::drawInstanced(const HBE::Mesh &mesh,
                                    const HBE::Material &material) {

    }

    void VK_Renderer::clear() const {

    }


}

