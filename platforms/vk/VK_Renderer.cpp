#include <core/resource/Resources.h>
#include <platforms/vk/raytracing/VK_RaytracingPipeline.h>
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

#include "VK_RasterizationPipeline.h"
#include "core/graphics/Graphics.h"
#include "core/utility/Profiler.h"
#include "VK_PipelineDescriptors.h"
#include "VK_RasterizationPipelineInstance.h"
#include "raytracing/VK_RaytracingPipelineInstance.h"
#include "core/graphics/RenderGraph.h"
#include "platforms/vk/VK_ComputePipeline.h"
#include "VK_ComputeInstance.h"

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


	void VK_Renderer::onWindowSizeChange(Window *window) {
		windowResized = true;
	}

	void VK_Renderer::reCreateSwapchain() {
		if (window->isMinimized()) {
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
		ui_render_target->setResolution(width, height);
		Graphics::onSwapchainResized.invoke(width, height);
	}

	void VK_Renderer::onWindowClosed() {
		device->wait();
	}

	void VK_Renderer::computeDispatch(ComputeDispatchCmdInfo &compute_dispatch_cmd_info) {
		VkMemoryBarrier memoryBarrier = {};
		memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		memoryBarrier.srcAccessMask =
				VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;  // Ensure all graphics memory accesses are complete
		memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;  // Prepare for compute shader access
		uint32_t extra_bits = device->getPhysicalDevice().getEnabledExtensionFlags() & EXTENSION_FLAG_RAY_TRACING_PIPELINE ? VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR : 0;
		vkCmdPipelineBarrier(
				command_pool->getCurrentBuffer(),
				VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | extra_bits,  // Wait for all graphics stages to complete
				VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,  // Ensure the compute shader starts after
				0,
				1,
				&memoryBarrier,
				0,
				nullptr,
				0,
				nullptr
		);
		HB_PROFILE_BEGIN("ComputeDispatch");
		const VK_ComputePipeline *vk_pipeline = dynamic_cast<const VK_ComputePipeline *>(compute_dispatch_cmd_info.pipeline_instance->getComputePipeline());
		vkCmdBindPipeline(command_pool->getCurrentBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, vk_pipeline->getHandle());
		compute_dispatch_cmd_info.pipeline_instance->bind();
		ivec3 workgroup_size = vk_pipeline->getWorkgroupSize();
		vkCmdDispatch(command_pool->getCurrentBuffer(),
		              ceil(compute_dispatch_cmd_info.size_x / workgroup_size.x),
		              ceil(compute_dispatch_cmd_info.size_y / workgroup_size.y),
		              ceil(compute_dispatch_cmd_info.size_z / workgroup_size.z));
		compute_dispatch_cmd_info.pipeline_instance->unbind();
		HB_PROFILE_END("ComputeDispatch");
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

	void VK_Renderer::rasterize(RasterizeCmdInfo &render_cmd_info) {
		//todo: create scene specific render graph
		HB_PROFILE_BEGIN("RenderPass");

		const VK_RenderPass *render_pass = dynamic_cast<const VK_RenderPass *>(render_cmd_info.render_target);

		vec2i resolution = render_pass->getResolution();
		VkViewport viewport{};

		if (RASTERIZE_CMD_FLAG_INVERSE_Y & render_cmd_info.flags) {
			viewport.x = 0.0f;
			viewport.y = static_cast<float>(resolution.y);
			viewport.width = static_cast<float>(resolution.x);
			viewport.height = -static_cast<float>(resolution.y);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
		} else {
			viewport.x = 0.0f;
			viewport.y = 0;
			viewport.width = static_cast<float>(resolution.x);
			viewport.height = static_cast<float>(resolution.y);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
		}


		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = VkExtent2D{(uint32_t) resolution.x, (uint32_t) resolution.y};

		//static_cast<VK_RenderTarget*>(render_target)->begin(command_pool->getCurrentBuffer());
		vkCmdSetViewport(command_pool->getCurrentBuffer(), 0, 1, &viewport);
		vkCmdSetScissor(command_pool->getCurrentBuffer(), 0, 1, &scissor);
		UniformBufferObject ubo{};
		ubo.view = render_cmd_info.view;
		ubo.projection = render_cmd_info.projection;

		render_pass->begin(command_pool->getCurrentBuffer(), current_frame);

		const std::vector<DrawCmdInfo> *render_cache_sorted = &render_cmd_info.render_graph->getRenderCache();
		const std::vector<DrawCmdInfo> *ordered_render_cache = &render_cmd_info.render_graph->getOrderedRenderCache();
		const std::vector<DrawCmdInfo> *caches[2];
		caches[0] = render_cache_sorted;
		caches[1] = ordered_render_cache;
		HB_PROFILE_BEGIN("RenderPassLoopDrawCmd");
		const RasterizationPipeline *last_pipeline = nullptr;
		RasterizationPipelineInstance *last_pipeline_instance = nullptr;
		const Mesh *last_mesh = nullptr;
		for (int cache_index = 0; cache_index < 2; ++cache_index) {
			const std::vector<DrawCmdInfo> &cache = *caches[cache_index];
			for (int i = 0; i < cache.size(); ++i) {
				const DrawCmdInfo &current_cmd = cache[i];
				if ((current_cmd.layer & render_cmd_info.layer_mask) != cache[i].layer) {
					continue;
				}
				const RasterizationPipeline *current_pipeline = current_cmd.pipeline_instance->getGraphicPipeline();
				RasterizationPipelineInstance *current_pipeline_instance = current_cmd.pipeline_instance;
				const Mesh *current_mesh = current_cmd.mesh;
				if (current_pipeline != last_pipeline) {
					current_pipeline->bind();
					last_pipeline = current_pipeline;
				}
				if (current_pipeline_instance != last_pipeline_instance) {
					current_pipeline_instance->setUniform("ubo", &ubo, current_frame);
					current_pipeline_instance->bind();
					last_pipeline_instance = current_pipeline_instance;
				}
				if (current_mesh != last_mesh) {
					current_mesh->bind();
					last_mesh = current_mesh;
				}
				for (int j = 0; j < current_cmd.push_constants_count; ++j) {
					current_cmd.pipeline_instance->getGraphicPipeline()->pushConstant(current_cmd.push_constants[j].name, current_cmd.push_constants[j].data);
				}
				if (current_cmd.mesh->hasIndexBuffer()) {
					vkCmdDrawIndexed(command_pool->getCurrentBuffer(), current_cmd.mesh->getIndexCount(), current_cmd.mesh->getInstanceCount(), 0, 0, 0);
				} else {
					vkCmdDraw(command_pool->getCurrentBuffer(), current_cmd.mesh->getVertexCount(), current_cmd.mesh->getInstanceCount(), 0, 0);
				}
				if (i != cache.size() - 1) {
					if (cache[i + 1].mesh != current_cmd.mesh) {
						current_cmd.mesh->unbind();
					}
					if (cache[i + 1].pipeline_instance != current_cmd.pipeline_instance) {
						current_cmd.pipeline_instance->unbind();
					}
					if (cache[i + 1].pipeline_instance->getGraphicPipeline() != current_cmd.pipeline_instance->getGraphicPipeline()) {
						current_cmd.pipeline_instance->getGraphicPipeline()->unbind();
					}
				} else {
					current_cmd.mesh->unbind();
					current_cmd.pipeline_instance->unbind();
					current_cmd.pipeline_instance->getGraphicPipeline()->unbind();
				}
			}
		}
		HB_PROFILE_END("RenderPassLoopDrawCmd");
		render_pass->end(command_pool->getCurrentBuffer());
		HB_PROFILE_END("RenderPass");
	}

	void VK_Renderer::traceRays(TraceRaysCmdInfo &trace_rays_cmd_info) {
		VkMemoryBarrier memoryBarrier = {};
		memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		memoryBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;  // Ensure all graphics memory accesses are complete
		memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;  // Prepare for compute shader access
		vkCmdPipelineBarrier(
				command_pool->getCurrentBuffer(),
				VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
				VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,  // Wait for all graphics stages to complete
				VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,  // Ensure the compute shader starts after
				0,
				1,
				&memoryBarrier,
				0,
				nullptr,
				0,
				nullptr
		);
		const VK_RaytracingPipelineInstance *vk_pipeline_instance = dynamic_cast<const VK_RaytracingPipelineInstance *>(trace_rays_cmd_info.pipeline_instance);
		const VK_RaytracingPipeline *vk_pipeline = vk_pipeline_instance->getPipeline();
		vk_pipeline->bind();
		vk_pipeline_instance->bind();

		device->vkCmdTraceRaysKHR(command_pool->getCurrentBuffer(),
		                          &vk_pipeline->getRaygenShaderBindingTable(),
		                          &vk_pipeline->getMissShaderBindingTable(),
		                          &vk_pipeline->getHitShaderBindingTable(),
		                          &vk_pipeline->getCallableShaderBindingTable(),
		                          trace_rays_cmd_info.resolution.x,
		                          trace_rays_cmd_info.resolution.y,
		                          1);
		vk_pipeline_instance->unbind();
		vk_pipeline->unbind();
	}

	void VK_Renderer::beginFrame() {
		HB_PROFILE_BEGIN("CommandPoolWait");
		command_pool->begin();
		HB_PROFILE_END("CommandPoolWait");
	}

	void VK_Renderer::present(PresentCmdInfo &present_cmd_info) {
		if (frame_presented) return;
		HB_ASSERT(frame_presented == false, "Frame already presented, call beginFrame() before present() and endFrame() after present()");
		HB_ASSERT(present_cmd_info.image_count <= 4 && present_cmd_info.image_count > 0, "layers should be from 1 to 4");
		HB_PROFILE_BEGIN("AquireImage");
		frame_presented = true;
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
		HB_PROFILE_END("AquireImage");
		HB_PROFILE_BEGIN("WaitImageInflight");
		if (images_in_flight_fences[current_image] != nullptr) {
			images_in_flight_fences[current_image]->wait();
		}
		HB_PROFILE_END("WaitImageInflight");
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

		VK_Image **vk_images = reinterpret_cast< VK_Image **>(present_cmd_info.images);
		for (int i = 0; i < present_cmd_info.image_count; ++i) {
			device->getAllocator()->cmdBarrierTransitionImageLayout(command_pool, vk_images[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		screen_pipeline_instance->setImageArray("layers", &present_cmd_info.images[0], present_cmd_info.image_count, current_frame, 0);
		screen_pipeline_instance->setUniform("ubo", &present_cmd_info.image_count);

		swapchain->beginRenderPass(current_image, command_pool->getCurrentBuffer());

		screen_pipeline->bind();
		screen_pipeline_instance->bind();


		vkCmdDraw(command_pool->getCurrentBuffer(), 3, 1, 0, 0);
		screen_pipeline_instance->unbind();
		screen_pipeline->unbind();

		swapchain->endRenderPass(command_pool->getCurrentBuffer());

		for (int i = 0; i < present_cmd_info.image_count; ++i) {
			device->getAllocator()->cmdBarrierTransitionImageLayout(command_pool, vk_images[i], vk_images[i]->getDesiredLayout());
		}

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

		HB_PROFILE_BEGIN("vkQueuePresentKHR");
		result = vkQueuePresentKHR(device->getQueue(QUEUE_FAMILY_PRESENT).getHandle(), &presentInfo);
		HB_PROFILE_END("vkQueuePresentKHR");
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || windowResized) {
			windowResized = false;
			reCreateSwapchain();
		} else if (result != VK_SUCCESS) {
			Log::error("failed to present swap chain image!");
		}

	}


	void VK_Renderer::endFrame() {
		HB_PROFILE_BEGIN("endFrame");

		if (!frame_presented) {
			Image *render_textures[1] = {
					main_render_target->getFramebufferTexture(current_frame)
			};
			PresentCmdInfo present_cmd_info{};
			present_cmd_info.images = render_textures;
			present_cmd_info.image_count = 1;
			present(present_cmd_info);
		}

		frame_presented = false;
		current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

		command_pool->getCurrentFence().wait();

		HB_PROFILE_END("endFrame");
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

	RenderTarget *VK_Renderer::getUIRenderTarget() {
		return ui_render_target;
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
		render_target_info.format = IMAGE_FORMAT_SRGBA8_NON_LINEAR;
		render_target_info.flags = RENDER_TARGET_FLAG_COLOR_ATTACHMENT | RENDER_TARGET_FLAG_DEPTH_ATTACHMENT | RENDER_TARGET_FLAG_CLEAR_COLOR | RENDER_TARGET_FLAG_CLEAR_DEPTH;

		main_render_target = Resources::createRenderTarget(render_target_info, "DEFAULT_RENDER_TARGET");

		render_target_info.flags = RENDER_TARGET_FLAG_COLOR_ATTACHMENT;
		ui_render_target = Resources::createRenderTarget(render_target_info, "UI_RENDER_TARGET");

		ShaderInfo shader_info{};
		shader_info.stage = SHADER_STAGE_VERTEX;
		shader_info.path = "shaders/defaults/TexturedFullScreenTriangle.vert";
		Shader *vert = new VK_Shader(device, shader_info);
		Resources::add("DEFAULT_SCREEN_VERT_SHADER", vert);

		shader_info.stage = SHADER_STAGE_FRAGMENT;
		shader_info.path = "shaders/defaults/TexturedFullScreenTriangle.frag";
		Shader *frag = new VK_Shader(device, shader_info);
		Resources::add("DEFAULT_SCREEN_FRAG_SHADER", frag);

		RasterizationPipelineInfo pipeline_info{};
		pipeline_info.vertex_shader = vert;
		pipeline_info.fragment_shader = frag;
		pipeline_info.attribute_info_count = 0;
		screen_pipeline = new VK_RasterizationPipeline(device, this, pipeline_info, swapchain->getRenderPass());
		Resources::add("DEFAULT_SCREEN_PIPELINE", screen_pipeline);

		RasterizationPipelineInstanceInfo screen_pipeline_instance_info{};
		screen_pipeline_instance_info.rasterization_pipeline = screen_pipeline;
		screen_pipeline_instance = new VK_RasterizationPipelineInstance(this, screen_pipeline_instance_info);
		Resources::add("DEFAULT_SCREEN_PIPELINE_INSTANCE", screen_pipeline_instance);
	}

	GraphicLimits VK_Renderer::getLimits() {
		VkPhysicalDeviceLimits device_limits = device->getPhysicalDevice().getProperties().limits;
		GraphicLimits limits{};
		limits.max_1D_texture_size = device_limits.maxImageDimension1D;
		limits.max_2D_texture_size = device_limits.maxImageDimension2D;
		limits.max_3D_texture_size = device_limits.maxImageDimension3D;

		limits.max_storage_buffer_size = device_limits.maxStorageBufferRange;
		return limits;
	}

	void VK_Renderer::waitCurrentFrame() {
		command_pool->getCurrentFence().wait();
	}

	void VK_Renderer::waitLastFrame() {
		command_pool->getLastFence().wait();
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

