//
// Created by Gabri on 2/27/2026.
//

#include "VK_Context.h"

#include "core/Application.h"
#include "core/utility/Log.h"

namespace HBE {
	HBE_RESULT VK_Context::init(const ContextInfo &info) {
		instance.init(info);
		surface.init(instance, Application::instance->getWindow()->getHandle());
		physical_device.init(instance, surface);
		device.init(this);
		allocator.init(this);
		swapchain.init(this);


		renderer.init(this);
		graphic_limits = renderer.getLimits();
		return HBE_RESULT_SUCCESS;
	}

	template<typename T, size_t I>
	void releaseStableHandleContainerObjects(StableHandleContainer<T, I> &container) {
		for (T &element: container) {
			if (element.allocated()) {
				Log::warning(std::string(typeid(T).name()) + " was not released properly before destroying vulkan context");
			}
		}
		container.clear();
	}

	HBE_RESULT VK_Context::release() {
		device.wait();
		renderer.release();
		allocator.processFreeRequests(0);
		swapchain.release();
		allocator.release();
		device.release();
		physical_device.release();
		surface.release();
		instance.release();

		releaseStableHandleContainerObjects(shaders);
		releaseStableHandleContainerObjects(pipeline_instances);
		releaseStableHandleContainerObjects(rasterization_pipelines);
		releaseStableHandleContainerObjects(raytracing_pipelines);
		releaseStableHandleContainerObjects(compute_pipelines);

		releaseStableHandleContainerObjects(rasterization_targets);
		releaseStableHandleContainerObjects(root_acceleration_structures);
		releaseStableHandleContainerObjects(aabb_acceleration_structures);
		releaseStableHandleContainerObjects(mesh_acceleration_structures);
		releaseStableHandleContainerObjects(meshes);
		releaseStableHandleContainerObjects(images);
		releaseStableHandleContainerObjects(texel_buffers);
		releaseStableHandleContainerObjects(buffers);
		releaseStableHandleContainerObjects(fences);

		return HBE_RESULT_SUCCESS;
	}
}
