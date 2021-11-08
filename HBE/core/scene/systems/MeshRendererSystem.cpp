
#include "MeshRendererSystem.h"
#include "core/graphics/Graphics.h"

#include <core/scene/Scene.h>

namespace HBE {
	MeshRendererSystem::MeshRendererSystem(Scene *scene) : System(scene) {
		scene->onDraw.subscribe(this, &MeshRendererSystem::draw);
	}

	void HBE::MeshRendererSystem::draw() {

		auto group = scene->group<Transform, MeshRenderer>();
		for (EntityHandle entity:group) {
			MeshRenderer &mesh_renderer = group.get<MeshRenderer>(entity);
			if (mesh_renderer.active) {
				Transform &transform = group.get<Transform>(entity);
				if (mesh_renderer.mesh && mesh_renderer.pipeline)
					Graphics::draw(transform, *mesh_renderer.mesh, *mesh_renderer.pipeline);
				else
					Log::warning("Mesh renderer does not have a material and/or a mesh assigned");
			}

		}

	}

	MeshRendererSystem::~MeshRendererSystem() {
		scene->onDraw.unsubscribe(this);
	}
}


