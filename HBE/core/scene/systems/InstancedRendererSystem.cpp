
#include "InstancedRendererSystem.h"
#include "core/graphics/Graphics.h"
#include "core/utility/Profiler.h"
#include <core/scene/Scene.h>

namespace HBE {
	InstancedRendererSystem::InstancedRendererSystem(Scene *scene) : System(scene) {
		scene->onDraw.subscribe(this, &InstancedRendererSystem::draw);
	}

	void HBE::InstancedRendererSystem::draw() {
		Profiler::begin("MeshRendererUpdate");
		Profiler::begin("MeshRendererUpdateGroup");
		auto group = scene->group<Transform, InstancedRenderer>();


		Profiler::end();
		transform_batches.clear();
		for (auto[handle, transform, instance_renderer]:group) {
			if (instance_renderer.active) {
				if (instance_renderer.mesh && instance_renderer.pipeline) {
					transform_batches.try_emplace(instance_renderer.pipeline, std::unordered_map<Mesh *, std::vector<mat4>>());
					transform_batches[instance_renderer.pipeline].try_emplace(instance_renderer.mesh, std::vector<mat4>());
					transform_batches[instance_renderer.pipeline][instance_renderer.mesh].emplace_back(transform.world());
				} else
					Log::warning("Mesh renderer does not have a material and/or a mesh assigned");
			}

		}

		for (const auto &pipeline_batch:transform_batches) {
			for (const auto &mesh_batch:pipeline_batch.second) {
				mesh_batch.first->setInstanceBuffer(1, mesh_batch.second.data(), mesh_batch.second.size());
				Graphics::drawInstanced(*mesh_batch.first, *pipeline_batch.first);
			}
		}
		Profiler::end();
	}


	InstancedRendererSystem::~InstancedRendererSystem() {
		scene->onDraw.unsubscribe(this);
	}

}

