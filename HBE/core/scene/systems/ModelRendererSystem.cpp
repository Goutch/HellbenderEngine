#include "ModelRendererSystem.h"
#include "core/scene/Scene.h"
#include "core/utility/Profiler.h"
#include "core/graphics/Graphics.h"

namespace HBE {
	ModelRendererSystem::ModelRendererSystem(Scene *scene) : System(scene) {
		scene->onDraw.subscribe(this, &ModelRendererSystem::draw);


	}

	void drawNode(const ModelNode *node, mat4 parent_transform) {
		mat4 transform = parent_transform * node->transform;
		for (int i = 0; i < node->primitives.size(); ++i) {
			const ModelPrimitive &primitive = node->primitives[i];
			if (primitive.mesh && primitive.pipeline)
				primitive.pipeline->setUniform("material", &primitive.material);
			Graphics::draw(transform, *primitive.mesh, *primitive.pipeline);
			else
			Log::warning("Model node does not have a material and/or a mesh assigned");
		}
		for (int i = 0; i < node->children.size(); ++i) {
			drawNode(node->children[i], transform);
		}
	}

	void ModelRendererSystem::draw() {

		Profiler::begin("ModelRendererUpdate");
		Profiler::begin("ModelRendererUpdateGroup");
		auto group = scene->group<Transform, ModelRenderer>();

		Profiler::end();
		for (auto[handle, transform, model_renderer]:group) {
			if (model_renderer.active) {
				for (const ModelNode *node:model_renderer.model->getNodes()) {
					drawNode(node, transform.world());
				}
			}
		}
		Profiler::end();
	}

	ModelRendererSystem::~ModelRendererSystem() {
		scene->onDraw.unsubscribe(this);
	}

}

