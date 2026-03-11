//
// Created by Gabri on 2/27/2026.
//

#include "VK_Context.h"

#include "core/Application.h"

namespace HBE
{
    void VK_Context::init(const ContextInfo& info)
    {
        instance.init(info);
        surface.init(instance, Application::instance->getWindow()->getHandle());
        physical_device.init(instance, surface);
        device.init(this);
        allocator.init(this);
        swapchain.init(this);

        renderer.init(this);
        graphic_limits = renderer.getLimits();


        renderer.
    }

    template <typename T, uint32_t I>
    void releaseStableHandleContainerObjects(StableHandleContainer<T, I>& container)
    {
        for (T& element : container)
        {
            if (element.allocated()) Log::warning("object was not released properly before destroying vulkan context");
            element.release();
        }
        container.clear();
    }

    void VK_Context::release()
    {
        allocator.processFreeRequests(0);

        releaseStableHandleContainerObjects(images);
        releaseStableHandleContainerObjects(shaders);
        releaseStableHandleContainerObjects(meshes);
        releaseStableHandleContainerObjects(rasterization_pipelines);
        releaseStableHandleContainerObjects(raytracing_pipelines);
        releaseStableHandleContainerObjects(compute_pipelines);
        releaseStableHandleContainerObjects(pipeline_instances);
        releaseStableHandleContainerObjects(rasterization_targets);
        releaseStableHandleContainerObjects(root_acceleration_structures);
        releaseStableHandleContainerObjects(aabb_acceleration_structures);
        releaseStableHandleContainerObjects(mesh_acceleration_structures);

        renderer.release();
        swapchain.release();
        allocator.release();
        device.release();
        physical_device.release();
        surface.release();
        surface.release();
        instance.release();
    }

}
