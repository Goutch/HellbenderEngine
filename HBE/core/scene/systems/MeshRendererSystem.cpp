
#include "MeshRendererSystem.h"
#include "core/graphics/Graphics.h"

#include <core/scene/Scene.h>
namespace HBE{
	MeshRendererSystem::MeshRendererSystem(Scene* scene): System(scene) {
		scene->onDraw.subscribe(this,&MeshRendererSystem::draw);
	}

	void HBE::MeshRendererSystem::draw() {

		auto group = scene->group<Transform, MeshRenderer>();
		for (EntityHandle entity:group) {
			MeshRenderer &mesh_renderer = group.get<MeshRenderer>(entity);
			if (mesh_renderer.active) {
				Transform &transform = group.get<Transform>(entity);

				Graphics::draw(transform, *mesh_renderer.mesh, *mesh_renderer.pipeline);
			}

		}

	}

	MeshRendererSystem::~MeshRendererSystem() {
		scene->onDraw.unsubscribe(this);
	}
}


