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
#include "VK_Allocator.h"
#include "VK_Mesh.h"
#include "core/resource/Material.h"
#include "VK_GraphicPipeline.h"
#include "VK_RenderTarget.h"

namespace HBE {
	struct UniformBufferObject {
		alignas(16) mat4 view;
		alignas(16) mat4 projection;
	};


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

		factory = new VK_ResourceFactory(this);

		Application::onWindowClosed.subscribe(this, &VK_Renderer::onWindowClosed);
		Configs::onVerticalSyncChange.subscribe(this,&VK_Renderer::reCreateSwapchain);
		window->onWindowSizeChange.subscribe(this, &VK_Renderer::onWindowSizeChange);
	}

	void VK_Renderer::onWindowSizeChange(int width, int height) {
		windowResized = true;
	}

	void VK_Renderer::reCreateSwapchain() {
		int width, height;
		window->getSize(width, height);
		device->wait();

		command_pool->clear();

		swapchain->recreate();

		command_pool->createCommandBuffers(MAX_FRAMES_IN_FLIGHT);
	}

	void VK_Renderer::onWindowClosed() {
		device->wait();
	}

	VK_Renderer::~VK_Renderer() {
		window->onWindowSizeChange.unsubscribe(this);
		Application::onWindowClosed.unsubscribe(this);
		Configs::onVerticalSyncChange.unsubscribe(this);
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

	void VK_Renderer::render(const RenderTarget *render_target,
							 const mat4 &projection_matrix,
							 const mat4 &view_matrix) {
		command_pool->begin(current_frame);

		int width,height;
		Graphics::getWindow()->getSize(width,height);
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = (float) height;
		viewport.width = (float) width;
		viewport.height = (float) -height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = VkExtent2D{(uint32_t) width, (uint32_t) height};

		//static_cast<VK_RenderTarget*>(render_target)->begin(command_pool->getCurrentBuffer());
		vkCmdSetViewport(command_pool->getCurrentBuffer(),0,1,&viewport);
		vkCmdSetScissor(command_pool->getCurrentBuffer(),0,1,&scissor);
		UniformBufferObject ubo{};
		ubo.view = view_matrix;
		ubo.projection = projection_matrix;
		swapchain->getRenderPass().begin(command_pool->getCurrentBuffer(), current_image);
		for (const auto &material_kv:render_cache) {
			const Material *material = material_kv.first;
			const VK_GraphicPipeline *pipeline = (VK_GraphicPipeline *) material->getPipeline();
			material->bind();
			pipeline->setUniform("ubo", &ubo);
			for (const auto &mesh_kv :material_kv.second) {
				const Mesh *mesh = mesh_kv.first;
				mesh->bind();
				for (const Transform *transform:mesh_kv.second) {
					pipeline->setUniform("constants", transform->getWorldMatrix());
					if (mesh->hasIndexBuffer()) {
						vkCmdDrawIndexed(command_pool->getCurrentBuffer(), mesh->getIndexCount(), 1, 0, 0, 0);
					} else {
						vkCmdDraw(command_pool->getCurrentBuffer(), mesh->getVertexCount(), 1, 0, 0);
					}
				}
				mesh->unbind();
			}
			material->unbind();
		}
		swapchain->getRenderPass().end(command_pool->getCurrentBuffer());
		command_pool->end(current_frame);
	}

	void VK_Renderer::beginFrame() {
		//wait for last frame i to finish
		frames_in_flight_fences[current_frame]->wait();

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
		render_cache.clear();
	}

	void VK_Renderer::draw(const Transform &transform, const Mesh &mesh, const Material &material) {
		auto it_material = render_cache.try_emplace(&material).first;
		auto it_mesh = it_material->second.try_emplace(&mesh).first;
		it_mesh->second.push_back(&transform);
	}

	void VK_Renderer::drawInstanced(const HBE::Mesh &mesh,
									const HBE::Material &material) {

	}

	void VK_Renderer::clear() const {

	}

	const VK_Swapchain &VK_Renderer::getSwapchain() const {
		return *swapchain;
	}

	VK_Device *VK_Renderer::getDevice() {
		return device;
	}

	VK_CommandPool *VK_Renderer::getCommandPool() {
		return command_pool;
	}

	uint32_t VK_Renderer::getCurrentFrame() const {
		return current_frame;
	}




}

