#include <core/graphics/Graphics.h>
#include "CameraSystem.h"
#include "core/scene/Scene.h"
#include "core/utility/Profiler.h"

namespace HBE {
	CameraSystem::CameraSystem(Scene *scene) : System(scene) {
		this->scene = scene;
		scene->onRender.subscribe(this, &CameraSystem::render);
		scene->onAttach<Camera>().subscribe(this, &CameraSystem::onCameraAttached);
		scene->onAttach<Camera2D>().subscribe(this, &CameraSystem::onCamera2DAttached);
		scene->onAttach<CameraUI>().subscribe(this, &CameraSystem::onCameraUIAttached);
	}

	void CameraSystem::onCameraAttached(Entity entity) {
		Camera &camera = entity.get<Camera>();
		if (camera.render_target == nullptr)
			camera.render_target = Graphics::getDefaultRenderTarget();
		camera.calculateProjection();

		if(!scene->getCameraEntity().valid())
		{
			scene->setCameraEntity(entity);
		}
	}

	void CameraSystem::onCamera2DAttached(Entity entity) {
		Camera2D &camera = entity.get<Camera2D>();
		if (camera.render_target == nullptr)
			camera.render_target = Graphics::getDefaultRenderTarget();
		camera.calculateProjection();

		if(!scene->getCameraEntity().valid())
		{
			scene->setCameraEntity(entity);
		}
	}

	void CameraSystem::onCameraUIAttached(Entity entity) {
		//CameraUI &camera = entity.get<CameraUI>();
		//if (camera.render_target == nullptr)
		//	camera.render_target = Graphics::getUIRenderTarget();
		//camera.calculateProjection();
	}

	void CameraSystem::render() {
		Profiler::begin("CameraRender");
		Profiler::begin("CameraRenderGroup");
		auto group = scene->group<Transform, Camera>();
		auto group2D = scene->group<Transform, Camera2D>();
		auto groupUI = scene->group<Transform, CameraUI>();

		RenderCmdInfo render_cmd_info{};
		Profiler::end();
		for (auto [handle, transform, camera]: group) {
			if (camera.active) {
				render_cmd_info.render_target = camera.render_target;
				render_cmd_info.projection = camera.projection;
				render_cmd_info.view = glm::inverse(transform.world());
				Graphics::render(render_cmd_info);
			}
		}
		for (auto [handle, transform, camera]: group2D) {
			if (camera.active) {
				render_cmd_info.render_target = camera.render_target;
				render_cmd_info.projection = camera.projection;
				render_cmd_info.view = glm::inverse(transform.world());
				Graphics::render(render_cmd_info);
			}
		}
		for (auto [handle, transform, camera]: groupUI) {
			if (camera.active) {
				render_cmd_info.render_target = camera.render_target;
				render_cmd_info.projection = camera.projection;
				render_cmd_info.view = glm::inverse(transform.world());
				Graphics::render(render_cmd_info);
			}
		}
		Profiler::end();
	}

	CameraSystem::~CameraSystem() {
		scene->onRender.unsubscribe(this);
		scene->onAttach<Camera>().unsubscribe(this);
	}


}
