
#include <core/graphics/Graphics.h>
#include "CameraSystem.h"
#include "core/scene/Scene.h"


namespace HBE {
	CameraSystem::CameraSystem(Scene *scene) : System(scene) {
		this->scene = scene;
		scene->onRender.subscribe(this, &CameraSystem::render);
		scene->onAttach<Camera>().subscribe(this, &CameraSystem::onCameraAttached);
		scene->onDetach<Camera>().subscribe(this, &CameraSystem::onCameraDetached);
	}

	void CameraSystem::onCameraAttached(Entity entity) {
		Camera &camera = entity.get<Camera>();
		if (camera.render_target == nullptr)
			camera.render_target = Graphics::getDefaultRenderTarget();
		camera.calculateProjection();
	}

	void CameraSystem::onCameraDetached(Entity entity) {
		Camera camera = entity.get<Camera>();
		if (camera.render_target == nullptr)
			camera.render_target = Graphics::getDefaultRenderTarget();
		camera.calculateProjection();
	}

	void CameraSystem::render() {

		auto group = scene->group<Transform, Camera>();

		for (EntityHandle handle:group) {
			Camera &camera = group.get<Camera>(handle);
			if (camera.active) {
				Transform &transform = group.get<Transform>(handle);
				Graphics::render(camera.render_target, camera.projection, glm::inverse(transform.world()));
			}
		}

	}

	CameraSystem::~CameraSystem() {
		scene->onRender.unsubscribe(this);
		scene->onAttach<Camera>().unsubscribe(this);
	}


}
