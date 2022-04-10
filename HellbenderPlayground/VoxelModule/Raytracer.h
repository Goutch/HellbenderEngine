#pragma once

#include "HBE.h"
#include "VoxelRendererSystem.h"

using namespace HBE;

class Raytracer {
public :
	static void init() {
		//-------------------SCENE CREATION--------------------------------------
		Scene &scene = *(new Scene());
		scene.addSystem(new VoxelRendererSystem(&scene));
		Application::setScene(&scene, true);

		Entity camera_entity = scene.createEntity("camera");
		Camera &camera = camera_entity.attach<Camera>();
		camera.render_target = Graphics::getDefaultRenderTarget();
		camera_entity.attach<CameraController>();

		scene.setCameraEntity(camera_entity);
	}
};
