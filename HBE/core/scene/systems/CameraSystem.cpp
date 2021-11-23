#include <core/graphics/Graphics.h>
#include "CameraSystem.h"
#include "core/scene/Scene.h"
#include "core/utility/Profiler.h"

namespace HBE {
	CameraSystem::CameraSystem(Scene *scene) : System(scene) {
		this->scene = scene;
		scene->onRender.subscribe(this, &CameraSystem::render);
		scene->onAttach<Camera>().subscribe(this, &CameraSystem::onCameraAttached);
		scene->onDetach<Camera>().subscribe(this, &CameraSystem::onCameraDetached);
	}

	void CameraSystem::onCameraAttached(Entity entity) {
		/*Camera &camera = entity.get<Camera>();
		if (camera.render_target == nullptr)
			camera.render_target = Graphics::getDefaultRenderTarget();
		camera.calculateProjection();*/
	}

	void CameraSystem::onCameraDetached(Entity entity) {
		/*Camera &camera = entity.get<Camera>();
		camera.calculateProjection();*/
	}

	void CameraSystem::render() {
		Profiler::begin("CameraRender");
		Profiler::begin("CameraRenderGroup");
		auto group = scene->group<Transform, Camera>();
		Profiler::end();
		for (auto [handle,transform,camera]:group) {
			if (camera.active) {
				Graphics::render(camera.render_target, camera.projection, glm::inverse(transform.world()));
			}
		}
		Profiler::end();
	}

	CameraSystem::~CameraSystem() {
		scene->onRender.unsubscribe(this);
		scene->onAttach<Camera>().unsubscribe(this);
	}


}
