#include "CameraSystem.h"
#include "core/scene/Scene.h"
#include "core/scene/components/Transform.h"
#include "core/scene/components/Camera.h"
#include "core/scene/components/Camera2D.h"
#include "core/scene/components/PixelCamera.h"
#include "core/scene/components/Node3D.h"

namespace HBE {
	CameraSystem::CameraSystem(Scene *scene) : System(scene) {
		this->scene = scene;
		scene->onRender.subscribe(this, &CameraSystem::render);
		scene->onAttach<Camera>().subscribe(this, &CameraSystem::onCameraAttached);
		scene->onAttach<Camera2D>().subscribe(this, &CameraSystem::onCamera2DAttached);
		scene->onAttach<PixelCamera>().subscribe(this, &CameraSystem::onPixelCameraAttached);
	}

	void CameraSystem::onCameraAttached(Entity entity) {
		Camera *camera = entity.get<Camera>();
		if (camera->getRenderTarget() == nullptr)
			camera->setRenderTarget(Graphics::getDefaultRenderTarget());

		if (!scene->getCameraEntity().valid()) {
			scene->setCameraEntity(entity);
		}
	}

	void CameraSystem::onCamera2DAttached(Entity entity) {
		Camera2D *camera = entity.get<Camera2D>();
		if (camera->getRenderTarget() == nullptr)
			camera->setRenderTarget(Graphics::getDefaultRenderTarget());

		if (!scene->getCameraEntity().valid()) {
			scene->setCameraEntity(entity);
		}
	}

	void CameraSystem::onPixelCameraAttached(Entity entity) {
		PixelCamera *camera = entity.get<PixelCamera>();
		if (camera->getRenderTarget() == nullptr)
			camera->setRenderTarget(Graphics::getDefaultRenderTarget());

		if (!scene->getCameraEntity().valid()) {
			scene->setCameraEntity(entity);
		}
	}

	void CameraSystem::render(RenderGraph *render_graph) {
		HB_PROFILE_BEGIN("CameraRender");
		HB_PROFILE_BEGIN("CameraRenderGroup");
		auto group = scene->group<Node3D, Transform, Camera>();
		auto group_2D = scene->group<Node3D, Transform, Camera2D>();
		auto group_pixel = scene->group<Node3D, Transform, PixelCamera>();
		RasterizeCmdInfo render_cmd_info{};
		render_cmd_info.render_graph = render_graph;
		HB_PROFILE_END("CameraRenderGroup");
		for (auto [handle, node, transform, camera]: group) {
			if (!node.isActiveInHierarchy()) {
				continue;
			}
			if (camera.active) {
				render_cmd_info.render_target = camera.getRenderTarget();
				render_cmd_info.projection = camera.projection;
				render_cmd_info.view = glm::inverse(transform.world());
				render_cmd_info.layer_mask = camera.layer_mask;
				Graphics::rasterize(render_cmd_info);
			}
		}
		for (auto [handle, node, transform, camera]: group_2D) {
			if (!node.isActiveInHierarchy()) {
				continue;
			}
			if (camera.active) {
				render_cmd_info.render_target = camera.getRenderTarget();
				render_cmd_info.projection = camera.projection;
				render_cmd_info.view = glm::inverse(transform.world());
				render_cmd_info.layer_mask = camera.layer_mask;
				Graphics::rasterize(render_cmd_info);
			}
		}
		for (auto [handle, node, transform, camera]: group_pixel) {
			if (!node.isActiveInHierarchy()) {
				continue;
			}
			if (camera.active) {
				render_cmd_info.render_target = camera.getRenderTarget();
				render_cmd_info.projection = camera.projection;
				render_cmd_info.view = glm::inverse(transform.world());
				render_cmd_info.layer_mask = camera.layer_mask;
				Graphics::rasterize(render_cmd_info);
			}
		}
		HB_PROFILE_END("CameraRender");
	}

	CameraSystem::~CameraSystem() {
		scene->onRender.unsubscribe(this);
		scene->onAttach<Camera>().unsubscribe(this);
	}
}
