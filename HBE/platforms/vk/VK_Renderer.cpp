#include "VK_Renderer.h"

#define GLFW_INCLUDE_VULKAN

#include "Configs.h"
#include "VK_Window.h"
#include "VK_Instance.h"
#include "VK_PhysicalDevice.h"
#include "VK_Swapchain.h"
#include "VK_Device.h"
#include "VK_Surface.h"
#include "VK_ResourceFactory.h"
#include "VK_RenderPass.h"
#include "VK_CommandPool.h"
#include "VK_Fence.h"
#include "Application.h"

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
        command_pool = new VK_CommandPool(device, MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            image_available_semaphores.push_back(new VK_Semaphore(*device));
            render_finished_semaphores.push_back(new VK_Semaphore(*device));
            frames_in_flight_fences.push_back(new VK_Fence(*device));
        }
        images_in_flight_fences.resize(swapchain->getRenderPass().getFrameBuffers().size(), nullptr);
        Application::onWindowClosed.subscribe(this, &VK_Renderer::onWindowClosed);
        window->onWindowSizeChange.subscribe(this, &VK_Renderer::onWindowSizeChange);

        factory = new VK_ResourceFactory(this);
    }

    void VK_Renderer::onWindowSizeChange(int width, int height) {
        windowResized = true;
    }

    void VK_Renderer::reCreateSwapchain() {
        int width, height;
        window->getSize(width, height);
        device->wait();

        command_pool->clear();
        delete swapchain;

        swapchain = new VK_Swapchain(width, height, surface->getHandle(), *device);

        command_pool->createCommandBuffers(MAX_FRAMES_IN_FLIGHT);
    }

    void VK_Renderer::onWindowClosed() {
        device->wait();
    }

    VK_Renderer::~VK_Renderer() {
        window->onWindowSizeChange.unsubscribe(this);
        Application::onWindowClosed.unsubscribe(this);
        device->wait();
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            delete image_available_semaphores[i];
            delete render_finished_semaphores[i];
            delete frames_in_flight_fences[i];
        }
        delete command_pool;

        delete factory;
        delete swapchain;
        delete device;
        delete physical_device;
        delete surface;
        delete instance;
    }

    const ResourceFactory *VK_Renderer::getResourceFactory() const {
        return factory;
    }

    void VK_Renderer::render(const HBE::RenderTarget *render_target,
                             const mat4 &projection_matrix,
                             const mat4 &view_matrix) {

    }

    void VK_Renderer::beginFrame() {
        //wait for last frame i to finish
        frames_in_flight_fences[current_frame]->wait();

        //getImageIndex
        VkResult result = vkAcquireNextImageKHR(device->getHandle(),
                                                swapchain->getHandle(),
                                                UINT64_MAX,
                                                image_available_semaphores[current_frame]->getHandle(),
                                                VK_NULL_HANDLE,
                                                &current_image);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            reCreateSwapchain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            Log::error("failed to acquire swap chain image!");
        }


        if (images_in_flight_fences[current_image] != nullptr) {
            images_in_flight_fences[current_image]->wait();
        }

        images_in_flight_fences[current_image] = frames_in_flight_fences[current_frame];


    }

    void VK_Renderer::present(const HBE::RenderTarget *render_target) {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {image_available_semaphores[current_frame]->getHandle()};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &command_pool->getBuffers()[current_frame];

        VkSemaphore signalSemaphores[] = {render_finished_semaphores[current_frame]->getHandle()};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        frames_in_flight_fences[current_frame]->reset();
        if (vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo,
                          frames_in_flight_fences[current_frame]->getHandle()) !=
            VK_SUCCESS) {
            Log::error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapchain->getHandle()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &current_image;
        presentInfo.pResults = nullptr; // Optional

        VkResult result = vkQueuePresentKHR(device->getPresentQueue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || windowResized) {
            windowResized = false;
            reCreateSwapchain();
            return;
        } else if (result != VK_SUCCESS) {
            Log::error("failed to present swap chain image!");
        }
    }

    void VK_Renderer::endFrame() {

        current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void VK_Renderer::draw(const HBE::Transform &transform,
                           const HBE::Mesh &mesh,
                           const HBE::Material &material) {
        command_pool->begin(current_frame);
        swapchain->getRenderPass().begin(command_pool->getCurrentBuffer(), current_image);

        material.getPipeline()->bind();
        mesh.bind();
        vkCmdDraw(command_pool->getCurrentBuffer(), static_cast<glm::uint32_t>(mesh.getVertexCount()), 1, 0, 0);
        mesh.unbind();
        material.getPipeline()->unbind();
        swapchain->getRenderPass().end(command_pool->getCurrentBuffer());
        command_pool->end(current_frame);
    }

    void VK_Renderer::drawInstanced(const HBE::Mesh &mesh,
                                    const HBE::Material &material) {

    }

    void VK_Renderer::clear() const {

    }

    const VK_Swapchain &VK_Renderer::getSwapchain() const {
        return *swapchain;
    }

    const VK_Device *VK_Renderer::getDevice() const {
        return device;
    }

    const VK_CommandPool *VK_Renderer::getCommandPool() const {
        return command_pool;
    }


}

