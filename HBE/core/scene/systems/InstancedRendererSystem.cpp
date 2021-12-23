
#include "InstancedRendererSystem.h"
#include "core/graphics/Graphics.h"
#include "core/utility/Profiler.h"
#include <core/scene/Scene.h>

namespace HBE {
	InstancedRendererSystem::InstancedRendererSystem(Scene *scene) : System(scene) {
		scene->onDraw.subscribe(this, &InstancedRendererSystem::draw);
	}

	void HBE::InstancedRendererSystem::draw() {
		//TODO:OPTIMIZE, METHOD IS VERY SLOW (MAYBE EVEN SLOWER THAN REGULAR DRAW)
		Profiler::begin("InstancedRendererUpdate");
		Profiler::begin("InstancedRendererUpdateGroup");
		auto group = scene->group<Transform, InstancedRenderer>();

		Profiler::end();
		transform_batches.clear();
		for (auto[handle, transform, instance_renderer]:group) {
			if (instance_renderer.active) {
				if (instance_renderer.mesh && instance_renderer.pipeline) {
					if(transform_batches.find(instance_renderer.pipeline)==transform_batches.end())
					{
						transform_batches.emplace(std::make_pair(instance_renderer.pipeline, std::unordered_map<Mesh *, std::vector<mat4>>()));
					}
					if(transform_batches[instance_renderer.pipeline].find(instance_renderer.mesh)==transform_batches[instance_renderer.pipeline].end())
					{
						transform_batches[instance_renderer.pipeline].emplace(std::make_pair(instance_renderer.mesh, std::vector<mat4>()));
					}
					transform_batches[instance_renderer.pipeline][instance_renderer.mesh].emplace_back(transform.world());
				} else
					Log::warning("Instance renderer does not have a material and/or a mesh assigned");
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


