
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
#if  defined(USE_ENTT) || defined(PERSISTENT)
		for (auto [handle,transform,mesh_renderer]:group) {
			if (mesh_renderer.active) {
				if (mesh_renderer.mesh && mesh_renderer.pipeline)
					Graphics::draw(transform.world(), *mesh_renderer.mesh, *mesh_renderer.pipeline);
				else
					Log::warning("Mesh renderer does not have a material and/or a mesh assigned");
			}

		}
#else

		Transform *transforms = scene->getAll<Transform>();
		MeshRenderer *renderers = scene->getAll<MeshRenderer>();
		for (size_t i = 0; i < group.count(); ++i) {

			auto [handle, transform, meshRenderer]=group.get((uint32_t)i);
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


