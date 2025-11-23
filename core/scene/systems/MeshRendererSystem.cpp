#include "MeshRendererSystem.h"
#include "core/graphics/Graphics.h"
#include "core/utility/Profiler.h"
#include <core/scene/Scene.h>
#include <core/graphics/RenderGraph.h>
#include "core/scene/components/Transform.h"
#include "core/scene/components/Node3D.h"

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
		auto group = scene->group<Node3D, Transform, MeshRenderer>();
		uint cached_push_constants_count = 0;
		PushConstantInfo* push_constant_infos = nullptr;
		for (auto [handle,node,transform, mesh_renderer] : group)
		{
			if (!mesh_renderer.active || !node.isActiveInHierarchy())
				continue;
			if (mesh_renderer.mesh && mesh_renderer.pipeline_instance)
			{
				mat4 world_mat = transform.world();
				uint32_t push_constants_count = mesh_renderer.push_constants_count + mesh_renderer.use_transform_matrix_as_push_constant;
				if (cached_push_constants_count < push_constants_count)
				{
					if (push_constant_infos != nullptr)
						delete[] push_constant_infos;
					push_constant_infos = new PushConstantInfo[push_constants_count];
					cached_push_constants_count = push_constants_count;
				}
				for (uint32_t i = 0; i < mesh_renderer.push_constants_count; i++)
				{
					push_constant_infos[i] = mesh_renderer.push_constants[i];
				}
				if (mesh_renderer.use_transform_matrix_as_push_constant)
				{
					push_constant_infos[push_constants_count - 1].size = sizeof(mat4);
					push_constant_infos[push_constants_count - 1].name = "constants";
					push_constant_infos[push_constants_count - 1].data = &world_mat;
				}
				draw_cmd.push_constants_count = push_constants_count;
				draw_cmd.push_constants = push_constant_infos;
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
