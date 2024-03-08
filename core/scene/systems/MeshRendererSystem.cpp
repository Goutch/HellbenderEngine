
#include "MeshRendererSystem.h"
#include "core/graphics/Graphics.h"
#include "core/utility/Profiler.h"
#include <core/scene/Scene.h>
#include <core/graphics/RenderGraph.h>
#include "core/scene/components/Transform.h"
#include "core/scene/components/EntityState.h"

namespace HBE {
	MeshRendererSystem::MeshRendererSystem(Scene *scene) : System(scene) {
		scene->onDraw.subscribe(this, &MeshRendererSystem::draw);
		scene->onDrawNode.subscribe(this, &MeshRendererSystem::drawSceneNode);
	}

	void HBE::MeshRendererSystem::draw(RenderGraph *render_graph) {
		HB_PROFILE_BEGIN("MeshRendererDraw");
		DrawCmdInfo draw_cmd{};

		auto group = scene->group<EntityState, Transform, MeshRenderer>();

		for (auto [handle, state, transform, mesh_renderer]: group) {
			if (!mesh_renderer.active || state.state == ENTITY_STATE_INACTIVE)
				continue;
			if (!mesh_renderer.ordered) {
				if (mesh_renderer.mesh && mesh_renderer.pipeline_instance) {
					PushConstantInfo push_constant_info{};
					push_constant_info.size = sizeof(mat4);
					push_constant_info.name = "constants";
					draw_cmd.push_constants_count = 1;
					draw_cmd.push_constants = &push_constant_info;
					draw_cmd.mesh = mesh_renderer.mesh;
					draw_cmd.pipeline_instance = mesh_renderer.pipeline_instance;
					draw_cmd.layer = mesh_renderer.layer;

					mat4 world_matrix = transform.world();
					push_constant_info.data = &world_matrix;
					render_graph->draw(draw_cmd);
				} else {
					Log::warning("Mesh renderer does not have a material and/or a mesh assigned");
				}

			}
		}

		HB_PROFILE_END("MeshRendererDraw");
	}

	MeshRendererSystem::~MeshRendererSystem() {
		scene->onDraw.unsubscribe(this);
	}

	void MeshRendererSystem::drawSceneNode(RenderGraph *render_graph, SceneNode &node) {
		if (node.entity.has<MeshRenderer>()) {
			DrawCmdInfo draw_cmd{};
			MeshRenderer *mesh_renderer = node.entity.get<MeshRenderer>();
			if (!mesh_renderer->active || !mesh_renderer->mesh || !mesh_renderer->pipeline_instance || !mesh_renderer->ordered) {
				return;
			}
			Transform *transform = node.entity.get<Transform>();

			mat4 world_mat = transform->world();

			PushConstantInfo push_constant_info{};
			push_constant_info.size = sizeof(mat4);
			push_constant_info.name = "constants";
			push_constant_info.data = &world_mat;
			draw_cmd.push_constants_count = 1;
			draw_cmd.push_constants = &push_constant_info;

			draw_cmd.mesh = mesh_renderer->mesh;
			draw_cmd.pipeline_instance = mesh_renderer->pipeline_instance;
			draw_cmd.layer = mesh_renderer->layer;
			draw_cmd.order_in_layer = node.getHierarchyOrder();
			draw_cmd.flags = DRAW_CMD_FLAG_ORDERED;

			mat4 world_matrix = transform->world();
			push_constant_info.data = &world_matrix;
			render_graph->draw(draw_cmd);
		}
	}
}