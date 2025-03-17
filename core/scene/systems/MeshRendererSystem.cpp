#include "MeshRendererSystem.h"
#include "core/graphics/Graphics.h"
#include "core/utility/Profiler.h"
#include <core/scene/Scene.h>
#include <core/graphics/RenderGraph.h>
#include "core/scene/components/Transform.h"
#include "core/scene/components/HierachyNode.h"

namespace HBE
{
	MeshRendererSystem::MeshRendererSystem(Scene* scene) : System(scene)
	{
		scene->onDraw.subscribe(this, &MeshRendererSystem::onDraw);
	}

	void MeshRendererSystem::onDraw(RenderGraph* render_graph)
	{
		HB_PROFILE_BEGIN("MeshRendererDraw");
		DrawCmdInfo draw_cmd{};
		auto group = scene->group<HierarchyNode, Transform, MeshRenderer>();
		for (auto [handle,node,transform, mesh_renderer] : group)
		{
			if (!mesh_renderer.active || !node.isActiveInHierarchy())
				continue;
			if (mesh_renderer.mesh && mesh_renderer.pipeline_instance)
			{
				mat4 world_mat = transform.world();
				PushConstantInfo push_constant_info{};
				if (mesh_renderer.use_transform_matrix_as_push_constant)
				{
					push_constant_info.size = sizeof(mat4);
					push_constant_info.name = "constants";
					push_constant_info.data = &world_mat;
					draw_cmd.push_constants_count = 1;
					draw_cmd.push_constants = &push_constant_info;
					push_constant_info.data = &world_mat;
				}
				draw_cmd.mesh = mesh_renderer.mesh;
				draw_cmd.pipeline_instance = mesh_renderer.pipeline_instance;
				draw_cmd.layer = mesh_renderer.layer;

				if (mesh_renderer.ordered)
				{
					draw_cmd.flags = DRAW_CMD_FLAG_ORDERED;
					draw_cmd.order_in_layer = node.getGlobalIndex();
				}
				else
				{
					draw_cmd.flags = DRAW_CMD_FLAG_NONE;
					draw_cmd.order_in_layer = 0;
				}

				mat4 world_matrix = transform.world();
				push_constant_info.data = &world_matrix;
				render_graph->add(draw_cmd);
			}
			else
			{
				Log::warning("Mesh renderer does not have a material and/or a mesh assigned");
			}
		}
		HB_PROFILE_END("MeshRendererDraw");
	}


	MeshRendererSystem::~MeshRendererSystem()
	{
		scene->onDraw.unsubscribe(this);
	}
}
