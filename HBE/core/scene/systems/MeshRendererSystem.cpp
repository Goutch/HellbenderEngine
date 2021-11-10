
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
		auto group = scene->group<Transform, MeshRenderer>();
		Profiler::end();
#ifdef USE_ENTT
		for (entity_handle entity:group) {
			MeshRenderer &mesh_renderer = group.get<MeshRenderer>(entity);
			if (mesh_renderer.active) {
				Transform &transform = group.get<Transform>(entity);
				if (mesh_renderer.mesh && mesh_renderer.pipeline)
					Graphics::draw(transform, *mesh_renderer.mesh, *mesh_renderer.pipeline);
				else
					Log::warning("Mesh renderer does not have a material and/or a mesh assigned");
			}

		}
#else

		Transform *transforms = scene->get<Transform>();
		MeshRenderer *renderers = scene->get<MeshRenderer>();
		for (size_t i = 0; i < group.size(); ++i) {
			if (renderers[i].active) {
				if (renderers[i].mesh && renderers[i].pipeline)
					Graphics::draw(transforms[i].world(), *renderers[i].mesh, *renderers[i].pipeline);
				else
					Log::warning("Mesh renderer does not have a material and/or a mesh assigned");
			}
		}
#endif
		Profiler::end();
	}

	MeshRendererSystem::~MeshRendererSystem() {
		scene->onDraw.unsubscribe(this);
	}
}


