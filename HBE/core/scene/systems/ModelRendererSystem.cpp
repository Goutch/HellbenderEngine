#include "ModelRendererSystem.h"
#include "core/scene/Scene.h"
#include "core/utility/Profiler.h"
#include "core/graphics/Graphics.h"

namespace HBE {
	ModelRendererSystem::ModelRendererSystem(Scene *scene) : System(scene) {
		scene->onUpdate.subscribe(this, &ModelRendererSystem::update);


	}

	void ModelRendererSystem::update(float delta) {

		Profiler::begin("ModelRendererUpdate");
		Profiler::begin("ModelRendererUpdateGroup");
		auto group = scene->group<Transform, ModelRenderer>();

		Profiler::end();
		for (auto[handle, transform, model_renderer]:group) {
			if (model_renderer.active) {
				for (const ModelNode &node:model_renderer.model->getNodes()) {
					if (node.mesh && node.pipeline)
						Graphics::draw(transform.world(), *node.mesh, *node.pipeline);
					else
						Log::warning("Model node does not have a material and/or a mesh assigned");

				}
			}
		}
		Profiler::end();
	}

	ModelRendererSystem::~ModelRendererSystem() {

	}

}

