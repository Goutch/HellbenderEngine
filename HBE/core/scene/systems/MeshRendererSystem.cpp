
#include "MeshRendererSystem.h"
#include "core/graphics/Graphics.h"
#include "core/utility/Profiler.h"
#include <core/scene/Scene.h>

namespace HBE {
	MeshRendererSystem::MeshRendererSystem(Scene *scene) : System(scene) {
		scene->onDraw.subscribe(this, &MeshRendererSystem::draw);
	}

	void HBE::MeshRendererSystem::draw() {
		Profiler::begin("MeshRendererUpdate");
		Profiler::begin("MeshRendererUpdateGroup");
		MeshRenderer& renderer=scene->get<MeshRenderer>(1);
		auto group = scene->group<Transform, MeshRenderer>();
		MeshRenderer& renderer2=scene->get<MeshRenderer>(1);
		Profiler::end();
		for (auto[handle, transform, mesh_renderer]:group) {
			if (mesh_renderer.active) {
				if (mesh_renderer.mesh && mesh_renderer.pipeline)
					Graphics::draw(transform.world(), *mesh_renderer.mesh, *mesh_renderer.pipeline);
				else
					Log::warning("Mesh renderer does not have a material and/or a mesh assigned");
			}

		}

		Profiler::end();
	}

	MeshRendererSystem::~MeshRendererSystem() {
		scene->onDraw.unsubscribe(this);
	}
}

