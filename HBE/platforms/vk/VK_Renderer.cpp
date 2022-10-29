#include <core/resource/Resources.h>
#include <platforms/vk/raytracing/VK_RaytracingPipeline.h>
#include "VK_Renderer.h"
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
#include "core/graphics/Graphics.h"
#include "core/utility/Profiler.h"
#include "VK_PipelineDescriptors.h"
#include "VK_GraphicPipelineInstance.h"
#include "raytracing/VK_RaytracingPipelineInstance.h"

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
		command_pool = new VK_CommandPool(*device, MAX_FRAMES_IN_FLIGHT, device->getQueue(QUEUE_FAMILY_GRAPHICS));

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {

			frames[i].image_available_semaphore = new VK_Semaphore(*device);
			frames[i].finished_semaphore = new VK_Semaphore(*device);
		}


		factory = new VK_ResourceFactory(this);

		images_in_flight_fences.resize(swapchain->getImagesCount(), nullptr);

		Application::onWindowClosed.subscribe(this, &VK_Renderer::onWindowClosed);
		Configs::onVerticalSyncChange.subscribe(this, &VK_Renderer::reCreateSwapchain);
		window->onSizeChange.subscribe(this, &VK_Renderer::onWindowSizeChange);
	}


	void VK_Renderer::onWindowSizeChange(uint32_t width, uint32_t height) {
		windowResized = true;
	}

	void VK_Renderer::reCreateSwapchain() {
		if(window->isMinimized()){
			return;
		}
		uint32_t width, height;
		window->getSize(width, height);

		device->wait();
		if (width == 0 || height == 0) {
			width = 1;
			height = 1;
		}
		for (size_t i = 0; i < swapchain->getImagesCount(); ++i) {
			images_in_flight_fences[i] = nullptr;
		}

		command_pool->clear();

		swapchain->recreate(width, height);
		//todo: check if nessesary
		command_pool->createCommandBuffers(MAX_FRAMES_IN_FLIGHT);

		main_render_target->setResolution(width, height);

		screen_material->setTexture("texture0", main_render_target->getFramebufferTexture(current_frame), current_frame, 0);
	}

	void VK_Renderer::onWindowClosed() {
		device->wait();
	}

	VK_Renderer::~VK_Renderer() {
		window->onSizeChange.unsubscribe(this);
		Application::onWindowClosed.unsubscribe(this);
		Configs::onVerticalSyncChange.unsubscribe(this);
		device->wait();
		Resources::destroyAll();
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			delete frames[i].image_available_semaphore;
			delete frames[i].finished_semaphore;
		}
		vkDestroySampler(device->getHandle(), default_sampler, VK_NULL_HANDLE);
		delete factory;
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

	void VK_Renderer::raytrace(const RootAccelerationStructure &root_acceleration_structure,
							   RaytracingPipelineInstance &pipeline,
							   const mat4 &projection_matrix,
							   const mat4 &view_matrix,
							   const vec2i resolution) {
		/*vec2i resolution = render_target.getResolution();
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(resolution.y);
		viewport.width = static_cast<float>(resolution.x);
		viewport.height = -static_cast<float>(resolution.y);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = VkExtent2D{(uint32_t) resolution.x, (uint32_t) resolution.y};

		vkCmdSetViewport(command_pool->getCurrentBuffer(), 0, 1, &viewport);
		vkCmdSetScissor(command_pool->getCurrentBuffer(), 0, 1, &scissor);*/

		VK_RaytracingPipelineInstance *vk_pipeline_instance = dynamic_cast<VK_RaytracingPipelineInstance *>(&pipeline);
		const VK_RaytracingPipeline *vk_pipeline = vk_pipeline_instance->getPipeline();
		UniformBufferObject ubo = {glm::inverse(view_matrix), glm::inverse(projection_matrix)};
		vk_pipeline_instance->setUniform("cam", &ubo, current_frame);
		vk_pipeline->bind();
		vk_pipeline_instance->bind();

		device->vkCmdTraceRaysKHR(command_pool->getCurrentBuffer(),
								  &vk_pipeline->getRaygenShaderBindingTable(),
								  &vk_pipeline->getMissShaderBindingTable(),
								  &vk_pipeline->getHitShaderBindingTable(),
								  &vk_pipeline->getCallableShaderBindingTable(),
								  resolution.x,
								  resolution.y,
								  1);
		//vkCmdPipelineBarrier(command_pool->getCurrentBuffer(), VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr, 0, nullptr, 0, nullptr);

		vk_pipeline_instance->unbind();
		vk_pipeline->unbind();
	}

	void VK_Renderer::render(const RenderTarget *render_target,
							 const mat4 &projection_matrix,
							 const mat4 &view_matrix) {
		Profiler::begin("RenderPass");
		const VK_RenderPass *render_pass = dynamic_cast<const VK_RenderPass *>(render_target);

		vec2i resolution = render_target->getResolution();
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(resolution.y);
		viewport.width = static_cast<float>(resolution.x);
		viewport.height = -static_cast<float>(resolution.y);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = VkExtent2D{(uint32_t) resolution.x, (uint32_t) resolution.y};

		//static_cast<VK_RenderTarget*>(render_target)->begin(command_pool->getCurrentBuffer());
		vkCmdSetViewport(command_pool->getCurrentBuffer(), 0, 1, &viewport);
		vkCmdSetScissor(command_pool->getCurrentBuffer(), 0, 1, &scissor);
		UniformBufferObject ubo{};
		ubo.view = view_matrix;
		ubo.projection = projection_matrix;
		render_pass->begin(command_pool->getCurrentBuffer(), current_frame);
		for (const auto &pipeline_kv: render_cache) {
			const GraphicPipeline *pipeline = pipeline_kv.first;
			pipeline->bind();
			for (const auto &material_kv:pipeline_kv.second) {
				GraphicPipelineInstance *material = material_kv.first;
				material->setUniform("ubo", &ubo, -1);
				material->bind();
				for (const auto &mesh_kv: material_kv.second) {
					const Mesh *mesh = mesh_kv.first;
					mesh->bind();
					for (const mat4 &transform_matrix: mesh_kv.second) {
						pipeline->pushConstant("constants", static_cast<const void *>(&transform_matrix[0]));
						if (mesh->hasIndexBuffer()) {
							vkCmdDrawIndexed(command_pool->getCurrentBuffer(), mesh->getIndexCount(), mesh->getInstanceCount(), 0, 0, 0);
						} else {
							vkCmdDraw(command_pool->getCurrentBuffer(), mesh->getVertexCount(), mesh->getInstanceCount(), 0, 0);
						}

					}
					mesh->unbind();
				}
				material->unbind();
			}
			pipeline->unbind();
		}
		for (const auto &pipeline_kv: instanced_cache) {
			const GraphicPipeline *pipeline = pipeline_kv.first;
			pipeline->bind();
			for (const auto &material_kv: pipeline_kv.second) {
				GraphicPipelineInstance *material = material_kv.first;
				material->setUniform("ubo", &ubo, -1);
				material->bind();

				std::vector<const Mesh *> meshes = material_kv.second;
				for (const Mesh *mesh : meshes) {
					mesh->bind();
					if (mesh->hasIndexBuffer()) {
						vkCmdDrawIndexed(command_pool->getCurrentBuffer(), mesh->getIndexCount(), mesh->getInstanceCount(), 0, 0, 0);
					} else {
						vkCmdDraw(command_pool->getCurrentBuffer(), mesh->getVertexCount(), mesh->getInstanceCount(), 0, 0);
					}
					mesh->unbind();
				}
			}
			pipeline->unbind();
		}
		render_pass->end(command_pool->getCurrentBuffer());
		Profiler::end();
	}

	void VK_Renderer::beginFrame() {
		//frames[current_frame].in_flight_fence->wait();
		//wait for last frame i to isEnd
		Profiler::begin("CommandPoolWait");
		command_pool->begin();
		Profiler::end();
	}

	void VK_Renderer::present(const Texture *image) {
		HB_ASSERT(frame_presented == false, "Frame already presented, call beginFrame() before present() and endFrame() after present()");
		Profiler::begin("AquireImage");

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
		Profiler::end();
		Profiler::begin("WaitImageInflight");
		if (images_in_flight_fences[current_image] != nullptr) {
			images_in_flight_fences[current_image]->wait();
		}
		Profiler::end();
		images_in_flight_fences[current_image] = &command_pool->getCurrentFence();
		command_pool->getCurrentFence().reset();


		vec2i resolution = vec2i(swapchain->getExtent().width, swapchain->getExtent().height);
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0;//static_cast<float>(resolution.y);
		viewport.width = static_cast<float>(resolution.x);
		viewport.height = static_cast<float>(resolution.y);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = VkExtent2D{(uint32_t) resolution.x, (uint32_t) resolution.y};

		//static_cast<VK_RenderTarget*>(render_target)->begin(command_pool->getCurrentBuffer());
		vkCmdSetViewport(command_pool->getCurrentBuffer(), 0, 1, &viewport);
		vkCmdSetScissor(command_pool->getCurrentBuffer(), 0, 1, &scissor);

		screen_material->setTexture("texture0", image, current_frame, 0);
		swapchain->beginRenderPass(current_image, command_pool->getCurrentBuffer());

		screen_pipeline->bind();
		screen_material->bind();
		vkCmdDraw(command_pool->getCurrentBuffer(), 3, 1, 0, 0);
		screen_material->unbind();
		screen_pipeline->unbind();

		swapchain->endRenderPass(command_pool->getCurrentBuffer());

		command_pool->end();
		VkSemaphore wait_semaphores[] = {frames[current_frame].image_available_semaphore->getHandle()};
		VkPipelineStageFlags stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		VkSemaphore signal_semaphores[] = {frames[current_frame].finished_semaphore->getHandle()};

		command_pool->submit(QUEUE_FAMILY_GRAPHICS,
							 wait_semaphores,
							 stages,
							 1,
							 signal_semaphores,
							 1);
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signal_semaphores;

		VkSwapchainKHR swapChains[] = {swapchain->getHandle()};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &current_image;
		presentInfo.pResults = nullptr; // Optional

		Profiler::begin("Present");
		result = vkQueuePresentKHR(device->getQueue(QUEUE_FAMILY_PRESENT).getHandle(), &presentInfo);
		Profiler::end();
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || windowResized) {
			windowResized = false;
			reCreateSwapchain();
		} else if (result != VK_SUCCESS) {
			Log::error("failed to present swap chain image!");
		}
		frame_presented = true;
	}


	void VK_Renderer::endFrame() {
		Profiler::begin("endFrame");

		if (!frame_presented) {
			present(main_render_target->getFramebufferTexture(current_frame));
		}

		frame_presented = false;
		current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
		onFrameChange.invoke(current_frame);
		render_cache.clear();
		Profiler::end();

	}

	void VK_Renderer::draw(mat4 transform_matrix, const Mesh &mesh, GraphicPipelineInstance &material) {
		auto pipeline_it = render_cache.find(material.getGraphicPipeline());
		if (pipeline_it == render_cache.end())
			pipeline_it = render_cache.emplace(material.getGraphicPipeline(), MAP(GraphicPipelineInstance*, MAP(const Mesh*, std::vector<mat4>))()).first;
		auto material_it = pipeline_it->second.find(&material);
		if (material_it == pipeline_it->second.end())
			material_it = pipeline_it->second.emplace(&material, MAP(const Mesh*, std::vector<mat4>)()).first;
		auto mesh_it = material_it->second.find(&mesh);
		if (mesh_it == material_it->second.end())
			mesh_it = material_it->second.emplace(&mesh, std::vector<mat4>()).first;

		mesh_it->second.emplace_back(transform_matrix);
	}

	void VK_Renderer::drawInstanced(const HBE::Mesh &mesh,
									GraphicPipelineInstance &material) {
		auto pipeline_it = instanced_cache.find(material.getGraphicPipeline());
		if (pipeline_it == instanced_cache.end())
			pipeline_it = instanced_cache.emplace(material.getGraphicPipeline(), MAP(GraphicPipelineInstance *, std::vector<const Mesh *>)()).first;
		auto material_it = pipeline_it->second.find(&material);
		if (material_it == pipeline_it->second.end())
			material_it = pipeline_it->second.emplace(&material, std::vector<const Mesh *>()).first;
		material_it->second.emplace_back(&mesh);
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

	RenderTarget *VK_Renderer::getDefaultRenderTarget() {
		return main_render_target;
	}

	void VK_Renderer::createDefaultResources() {
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_NEAREST;//If the object is close to the camera,
		samplerInfo.minFilter = VK_FILTER_NEAREST;//If the object is further from the camera
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

		samplerInfo.anisotropyEnable = device->getPhysicalDevice().getFeatures().samplerAnisotropy;
		samplerInfo.maxAnisotropy = device->getPhysicalDevice().getProperties().limits.maxSamplerAnisotropy;

		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;

		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.minLod = 0.0f; // 0 when close the camera.
		samplerInfo.maxLod = static_cast<float>(0);
		samplerInfo.mipLodBias = 0.0f; // Optional

		if (vkCreateSampler(device->getHandle(), &samplerInfo, nullptr, &default_sampler) != VK_SUCCESS) {
			Log::error("failed to create texture sampler!");
		}


		RenderTargetInfo render_target_info{};
		render_target_info.width = swapchain->getExtent().width;
		render_target_info.height = swapchain->getExtent().height;
		render_target_info.clear_color = vec4(0.f, 0.f, 0.f, 1.f);
		render_target_info.format = IMAGE_FORMAT_RGBA32F;
		render_target_info.flags = RENDER_TARGET_FLAG_DEPTH_TEST;
		main_render_target = Resources::createRenderTarget(render_target_info, "DEFAULT_RENDER_TARGET");

		ShaderInfo shader_info{};
		shader_info.stage = SHADER_STAGE_VERTEX;
		shader_info.path = "shaders/defaults/TexturedFullScreenTriangle.vert";
		Shader *vert = new VK_Shader(device, shader_info);
		Resources::add("DEFAULT_SCREEN_VERT_SHADER", vert);

		shader_info.stage = SHADER_STAGE_FRAGMENT;
		shader_info.path = "shaders/defaults/TexturedFullScreenTriangle.frag";
		Shader *frag = new VK_Shader(device, shader_info);
		Resources::add("DEFAULT_SCREEN_FRAG_SHADER", frag);

		GraphicPipelineInfo pipeline_info{};
		pipeline_info.vertex_shader = vert;
		pipeline_info.fragement_shader = frag;
		screen_pipeline = new VK_GraphicPipeline(device, this, pipeline_info, swapchain->getRenderPass());
		Resources::add("DEFAULT_SCREEN_PIPELINE", screen_pipeline);

		GraphicPipelineInstanceInfo screen_material_info{};
		screen_material_info.graphic_pipeline = screen_pipeline;
		screen_material = new VK_GraphicPipelineInstance(this, screen_material_info);
		Resources::add("DEFAULT_SCREEN_MATERIAL", screen_material);
	}

	void VK_Renderer::waitCurrentFrame() {
		command_pool->getCurrentFence().wait();
	}

	uint32_t VK_Renderer::getFrameCount() const {
		return MAX_FRAMES_IN_FLIGHT;
	}

	void VK_Renderer::waitAll() {
		device->wait();
	}

	VkSampler VK_Renderer::getDefaultSampler() {
		return default_sampler;
	}


}

