#include <core/resource/Resources.h>
#include "VK_Renderer.h"
#include "VK_Semaphore.h"
#include "VK_Fence.h"
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

#include "VK_GraphicPipeline.h"
#include "VK_RenderTarget.h"
#include "core/graphics/Graphics.h"

namespace HBE {
	struct UniformBufferObject {
		alignas(16) mat4 view;
		alignas(16) mat4 projection;
	};


	VK_Renderer::VK_Renderer() {
		window = dynamic_cast<VK_Window *>(Graphics::getWindow());

		uint32_t width, height;
		window->getSize(width, height);

		instance = new VK_Instance();
		surface = new VK_Surface(instance->getHandle(), window->getHandle());
		physical_device = new VK_PhysicalDevice(instance->getHandle(), surface->getHandle());
		device = new VK_Device(*physical_device);
		swapchain = new VK_Swapchain(width, height, surface->getHandle(), *device);
		command_pool = new VK_CommandPool(device, MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {

			frames[i].image_available_semaphore=new VK_Semaphore(*device);
			frames[i].finished_semaphore=new VK_Semaphore(*device);
			frames[i].in_flight_fence=new VK_Fence(*device);
		}


		factory = new VK_ResourceFactory(this);

		swapchain_render_pass = new VK_RenderPass(this);

		images_in_flight_fences.resize(swapchain_render_pass->getFrameBuffers().size(), nullptr);

		createDefaultResources();

		Application::onWindowClosed.subscribe(this, &VK_Renderer::onWindowClosed);
		Configs::onVerticalSyncChange.subscribe(this, &VK_Renderer::reCreateSwapchain);
		window->onSizeChange.subscribe(this, &VK_Renderer::onWindowSizeChange);
	}

	void VK_Renderer::onWindowSizeChange(uint32_t width, uint32_t height) {
		windowResized = true;
	}

	void VK_Renderer::reCreateSwapchain() {
		uint32_t width, height;
		window->getSize(width, height);
		device->wait();

		command_pool->clear();

		swapchain->recreate();

		swapchain_render_pass->recreate();

		command_pool->createCommandBuffers(MAX_FRAMES_IN_FLIGHT);
	}

	void VK_Renderer::onWindowClosed() {
		device->wait();
	}

	VK_Renderer::~VK_Renderer() {
		window->onSizeChange.unsubscribe(this);
		Application::onWindowClosed.unsubscribe(this);
		Configs::onVerticalSyncChange.unsubscribe(this);
		device->wait();
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			delete frames[i].image_available_semaphore;
			delete frames[i].finished_semaphore;
			delete frames[i].in_flight_fence;
		}
		delete factory;
		delete swapchain_render_pass;
		delete command_pool;
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

		uint32_t width, height;
		render_target->getResolution(width, height);
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(height);
		viewport.width = static_cast<float>(width);
		viewport.height = -static_cast<float>(height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = VkExtent2D{(uint32_t) width, (uint32_t) height};

		//static_cast<VK_RenderTarget*>(render_target)->begin(command_pool->getCurrentBuffer());
		vkCmdSetViewport(command_pool->getCurrentBuffer(), 0, 1, &viewport);
		vkCmdSetScissor(command_pool->getCurrentBuffer(), 0, 1, &scissor);
		UniformBufferObject ubo{};
		ubo.view = view_matrix;
		ubo.projection = projection_matrix;
		swapchain_render_pass->begin(command_pool->getCurrentBuffer(), current_image);
		for (const auto &pipeline_kv:render_cache) {
			GraphicPipeline *pipeline = pipeline_kv.first;
			VK_GraphicPipeline *vk_pipeline = (VK_GraphicPipeline *) pipeline;
			vk_pipeline->bind();
			pipeline->setUniform("ubo", &ubo);
			for (const auto &mesh_kv :pipeline_kv.second) {
				const Mesh *mesh = mesh_kv.first;
				mesh->bind();
				for (const Transform *transform:mesh_kv.second) {
					pipeline->pushConstant("constants", static_cast<void *>(&transform->getWorldMatrix()[0]));
					if (mesh->hasIndexBuffer()) {
						vkCmdDrawIndexed(command_pool->getCurrentBuffer(), mesh->getIndexCount(), 1, 0, 0, 0);
					} else {
						vkCmdDraw(command_pool->getCurrentBuffer(), mesh->getVertexCount(), 1, 0, 0);
					}
				}
				mesh->unbind();
			}
			vk_pipeline->unbind();
		}
		swapchain_render_pass->end(command_pool->getCurrentBuffer());
		command_pool->end(current_frame);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount=1;
		submitInfo.pCommandBuffers=&command_pool->getBuffers()[current_frame];

		frames[current_frame].in_flight_fence->reset();
		if (vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo,
						  frames[current_frame].in_flight_fence->getHandle()) != VK_SUCCESS) {
			Log::error("failed to submit draw command buffer!");
		}
	}

	void VK_Renderer::beginFrame() {
		//wait for last frame i to finish


	}

	void VK_Renderer::present(const HBE::RenderTarget *render_target) {
		frames[current_frame].in_flight_fence->wait();

		VkResult result = vkAcquireNextImageKHR(device->getHandle(),
												swapchain->getHandle(),
												UINT64_MAX,
												frames[current_frame].image_available_semaphore->getHandle(),
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

		images_in_flight_fences[current_image] = frames[current_frame].in_flight_fence;

		VK_RenderPass* current_render_pass=(VK_RenderPass*) render_target;

		screen_pipeline->setTexture("texture0",current_render_pass->getImage(current_frame));

		command_pool->begin(current_frame);

		uint32_t width, height;
		render_target->getResolution(width, height);
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(height);
		viewport.width = static_cast<float>(width);
		viewport.height = -static_cast<float>(height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = VkExtent2D{(uint32_t) width, (uint32_t) height};


		//static_cast<VK_RenderTarget*>(render_target)->begin(command_pool->getCurrentBuffer());
		vkCmdSetViewport(command_pool->getCurrentBuffer(), 0, 1, &viewport);
		vkCmdSetScissor(command_pool->getCurrentBuffer(), 0, 1, &scissor);

		swapchain_render_pass->begin(command_pool->getCurrentBuffer(), current_image);

		screen_pipeline->bind();
		vkCmdDraw(command_pool->getCurrentBuffer(), 3, 1, 0, 0);
		screen_pipeline->unbind();

		swapchain_render_pass->end(command_pool->getCurrentBuffer());
		command_pool->end(current_frame);


		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = {frames[current_frame].image_available_semaphore->getHandle()};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &command_pool->getBuffers()[current_frame];

		VkSemaphore signalSemaphores[] = {frames[current_frame].finished_semaphore->getHandle()};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		frames[current_frame].in_flight_fence->reset();
		if (vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo,
						  frames[current_frame].in_flight_fence->getHandle()) != VK_SUCCESS) {
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

		result = vkQueuePresentKHR(device->getPresentQueue(), &presentInfo);

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

	void VK_Renderer::draw(const Transform &transform, const Mesh &mesh, GraphicPipeline &pipeline) {
		auto it_material = render_cache.try_emplace(&pipeline).first;
		auto it_mesh = it_material->second.try_emplace(&mesh).first;
		it_mesh->second.push_back(&transform);
	}

	void VK_Renderer::drawInstanced(const HBE::Mesh &mesh,
									GraphicPipeline &pipeline) {

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

	const VK_RenderPass &VK_Renderer::getRenderPass() const {
		return *swapchain_render_pass;
	}

	RenderTarget *VK_Renderer::getMainRenderTarget() {
		return swapchain_render_pass;
	}

	void VK_Renderer::createDefaultResources() {
		uint8_t *texture_data=new uint8_t[16]{255, 0, 0, 255,
											  0, 255, 0, 255,
											  0, 0, 255, 255,
											  0, 0, 0, 255};
		TextureInfo texture_info{};
		texture_info.data = texture_data;
		texture_info.width = 2;
		texture_info.height = 2;
		texture_info.format = IMAGE_FORMAT_RGBA8;
		texture_info.flags = IMAGE_FLAG_NONE;
		Resources::add("DEFAULT_TEXTURE",factory->createTexture(texture_info));
		delete texture_data;

		ShaderInfo shader_info{};
		shader_info.stage = SHADER_STAGE_VERTEX;
		shader_info.path = "../../res/shaders/VK_Screen.vert";
		Shader *vert = new VK_Shader(device, shader_info);
		Resources::add("DEFAULT_SCREEN_VERT_SHADER",vert);

		shader_info.stage = SHADER_STAGE_FRAGMENT;
		shader_info.path = "../../res/shaders/VK_Screen.frag";
		Shader *frag = new VK_Shader(device, shader_info);
		Resources::add("DEFAULT_SCREEN_FRAG_SHADER",frag);

		GraphicPipelineInfo pipeline_info{};
		pipeline_info.vertex_shader =vert;
		pipeline_info.fragement_shader =frag;
		screen_pipeline = new VK_GraphicPipeline(device,this,pipeline_info);
		Resources::add("DEFAULT_SCREEN_PIPELINE",screen_pipeline);
	}


}

