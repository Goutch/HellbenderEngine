

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
#include "vulkan/vulkan.h"
#include "VK_RenderPass.h"
#include "VK_CommandPool.h"

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
        render_pass = new VK_RenderPass(device, swapchain);
        command_pool = new VK_CommandPool(device, render_pass->getFrameBuffers().size());
        factory = new VK_ResourceFactory(device, this);
        image_available_semaphore = new VK_Semaphore(*device);
        render_finished_semaphore = new VK_Semaphore(*device);
    }


    VK_Renderer::~VK_Renderer() {
        device->wait();
        delete render_finished_semaphore;
        delete image_available_semaphore;
        delete command_pool;
        delete render_pass;
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
        uint32_t imageIndex;
        vkAcquireNextImageKHR(device->getHandle(), swapchain->getHandle(), UINT64_MAX,
                              image_available_semaphore->getHandle(), VK_NULL_HANDLE, &imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {image_available_semaphore->getHandle()};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &command_pool->getBuffers()[imageIndex];

        VkSemaphore signalSemaphores[] = {render_finished_semaphore->getHandle()};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }


        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapchain->getHandle()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr; // Optional

        vkQueuePresentKHR(device->getPresentQueue(), &presentInfo);

        vkQueueWaitIdle(device->getPresentQueue());

    }

    void VK_Renderer::clearDrawCache() {

    }

    void VK_Renderer::draw(const HBE::Transform &transform,
                           const HBE::Mesh &mesh,
                           const HBE::Material &material) {
        const std::vector<VkCommandBuffer> &command_buffers = command_pool->getBuffers();
        for (size_t i = 0; i < command_buffers.size(); ++i) {
            current_command_buffer = &command_buffers[i];
            command_pool->begin(i);
            render_pass->begin(command_buffers[i], i);
            material.getPipeline()->bind();
            vkCmdDraw(command_pool->getBuffers()[i], 3, 1, 0, 0);
            material.getPipeline()->unbind();
            render_pass->end(command_buffers[i]);
            command_pool->end(i);
        }
    }

    void VK_Renderer::drawInstanced(const HBE::Mesh &mesh,
                                    const HBE::Material &material) {

    }

    void VK_Renderer::clear() const {

    }

    const VK_RenderPass &VK_Renderer::getRenderPass() const {
        return *render_pass;
    }

    const VkCommandBuffer *VK_Renderer::getCurrentCommandBuffer() const {
        return current_command_buffer;
    }


}

