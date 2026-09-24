#include "CameraSystem.h"
#include "core/scene/Scene.h"
#include "core/scene/components/Transform.h"
#include "core/scene/components/Camera.h"
#include "core/scene/components/Camera2D.h"
#include "core/scene/components/PixelCamera.h"
#include "core/scene/components/Node.h"

namespace HBE
{
    CameraSystem::CameraSystem(Scene* scene) : System(scene)
    {
        this->scene = scene;
        scene->onRender.subscribe(on_render_subscription_id, this, &CameraSystem::render);
        scene->onAttach<Camera>().subscribe(on_attach_3D_subscription_id, this, &CameraSystem::onCameraAttached);
        scene->onAttach<Camera2D>().subscribe(on_attach_2D_subscription_id, this, &CameraSystem::onCamera2DAttached);
        scene->onAttach<PixelCamera>().subscribe(on_attach_pixel_subscription_id, this, &CameraSystem::onPixelCameraAttached);
    }

    void CameraSystem::onCameraAttached(Entity entity)
    {
        Camera* camera = entity.get<Camera>();
        if (camera->render_target == HBE_NULL_HANDLE)
        {
            RendererResources resources;
            context.rendererGetResources(resources);
            camera->render_target = resources.main_render_target;
        }


        if (!scene->getCameraEntity().valid())
        {
            scene->setCameraEntity(entity);
        }
    }

    void CameraSystem::onCamera2DAttached(Entity entity)
    {
        Camera2D* camera = entity.get<Camera2D>();
        if (camera->render_target == HBE_NULL_HANDLE)
        {
            RendererResources resources;
            context.rendererGetResources(resources);
            camera->render_target = resources.main_render_target;
        }

        if (!scene->getCameraEntity().valid())
        {
            scene->setCameraEntity(entity);
        }
    }

    void CameraSystem::onPixelCameraAttached(Entity entity)
    {
        PixelCamera* camera = entity.get<PixelCamera>();
        if (camera->render_target == HBE_NULL_HANDLE)
        {
            RendererResources resources;
            context.rendererGetResources(resources);
            camera->render_target = resources.main_render_target;
        }

        if (!scene->getCameraEntity().valid())
        {
            scene->setCameraEntity(entity);
        }
    }

    float aspectRatio(vec2u resolution)
    {
        return static_cast<float>(resolution.x) / static_cast<float>(resolution.y);
    }

    void CameraSystem::render(RenderGraph* render_graph)
    {
        HB_PROFILE_BEGIN("CameraRender");
        HB_PROFILE_BEGIN("CameraRenderGroup");
        auto group = scene->group<Node, Transform, Camera>();
        auto group_2D = scene->group<Node, Transform, Camera2D>();
        auto group_pixel = scene->group<Node, Transform, PixelCamera>();
        RasterizeGraphCmdInfo raster_cmd_info{};
        raster_cmd_info.render_graph = render_graph;
        HB_PROFILE_END("CameraRenderGroup");
        for (auto [handle, node, transform, camera] : group)
        {
            if (!node.isActiveInHierarchy() || !camera.active || camera.render_target == HBE_NULL_HANDLE)
            {
                continue;
            }

            vec2u resolution;
            context.getRasterizationTargetResolution(camera.render_target, resolution);
            camera.calculateAspectRatio(resolution);
            if (camera.isProjectionDirty())
                camera.calculateProjection();

            raster_cmd_info.rasterization_target_handle = camera.render_target;
            raster_cmd_info.projection = camera.projection;
            raster_cmd_info.view = glm::inverse(transform.world());
            raster_cmd_info.layer_mask = camera.layer_mask;
            context.cmdRasterizeGraph(raster_cmd_info);
        }
        for (auto [handle, node, transform, camera] : group_2D)
        {
            if (!node.isActiveInHierarchy() || !camera.active || camera.render_target == HBE_NULL_HANDLE)
            {
                continue;
            }
            vec2u resolution;
            context.getRasterizationTargetResolution(camera.render_target, resolution);
            camera.calculateAspectRatio(resolution);
            if (camera.isProjectionDirty())
                camera.calculateProjection();

            raster_cmd_info.rasterization_target_handle = camera.render_target;
            raster_cmd_info.projection = camera.projection;
            raster_cmd_info.view = glm::inverse(transform.world());
            raster_cmd_info.layer_mask = camera.layer_mask;
            context.cmdRasterizeGraph(raster_cmd_info);
        }
        for (auto [handle, node, transform, camera] : group_pixel)
        {
            if (!node.isActiveInHierarchy() || !camera.active || camera.render_target == HBE_NULL_HANDLE)
            {
                continue;
            }

            vec2u resolution;
            context.getRasterizationTargetResolution(camera.render_target, resolution);
            camera.setResolution(resolution);
            if (camera.isProjectionDirty())
                camera.calculateProjection();
            raster_cmd_info.rasterization_target_handle = camera.render_target;
            raster_cmd_info.projection = camera.projection;
            raster_cmd_info.view = glm::inverse(transform.world());
            raster_cmd_info.layer_mask = camera.layer_mask;
            context.cmdRasterizeGraph(raster_cmd_info);
        }
        HB_PROFILE_END("CameraRender");
    }

    CameraSystem::~CameraSystem()
    {
        scene->onRender.unsubscribe(on_render_subscription_id);
        scene->onAttach<Camera>().unsubscribe(on_attach_3D_subscription_id);
        scene->onAttach<Camera2D>().unsubscribe(on_attach_2D_subscription_id);
        scene->onAttach<PixelCamera>().unsubscribe(on_attach_pixel_subscription_id);
    }
}
