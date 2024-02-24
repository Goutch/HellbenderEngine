
#include "ModelRendererSystem.h"
#include "core/scene/Scene.h"
#include "core/scene/components/EntityState.h"

namespace HBE {
	ModelRendererSystem::ModelRendererSystem(Scene *scene) : System(scene) {
		scene->onDraw.subscribe(this, &ModelRendererSystem::draw);
	}

	void drawNode(RenderGraph *render_graph, const ModelNode &node, mat4 parent_transform) {
		mat4 transform = parent_transform * node.transform;

		DrawCmdInfo draw_cmd{};
		PushConstantInfo push_constant_info{};
		push_constant_info.size = sizeof(mat4);
		push_constant_info.name = "constants";
		draw_cmd.push_constants_count = 1;
		draw_cmd.push_constants = &push_constant_info;
		push_constant_info.data = &transform;
		for (int i = 0; i < node.primitives.size(); ++i) {
			const ModelPrimitive &primitive = node.primitives[i];
			if (primitive.mesh != nullptr && primitive.material != nullptr) {
				draw_cmd.mesh = primitive.mesh;
				draw_cmd.pipeline_instance = primitive.material;

				render_graph->draw(draw_cmd);
			} else {
				Log::warning("Model node does not have a material and/or a mesh assigned");
			}
		}
		for (int i = 0; i < node.children.size(); ++i) {
			drawNode(render_graph, node.children[i], transform);
		}
	}

	void ModelRendererSystem::draw(RenderGraph *render_graph) {

		HB_PROFILE_BEGIN("ModelRendererUpdate");
		HB_PROFILE_BEGIN("ModelRendererUpdateGroup");
		auto group = scene->group<EntityState, Transform, ModelRenderer>();
		HB_PROFILE_END("ModelRendererUpdateGroup");
		for (auto [handle, state, transform, model_renderer]: group) {

			if (!model_renderer.active || state.state == ENTITY_STATE_INACTIVE)
				continue;

			for (const ModelNode &node: model_renderer.model->getNodes()) {
				drawNode(render_graph, node, transform.world());
			}
		}


		HB_PROFILE_END("ModelRendererUpdate");
	}

	ModelRendererSystem::~ModelRendererSystem() {
		scene->onDraw.unsubscribe(this);
	}
}