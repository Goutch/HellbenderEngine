//
// Created by User on 2/9/2024.
//

#include "UIPanelSystem.h"
#include "core/scene/systems/MeshRendererSystem.h"
#include "core/scene/Scene.h"
#include "core/resource/Resources.h"
#include "core/scene/components/Node3D.h"
#include "core/utility/Geometry.h"

namespace HBE
{
	UIPanelSystem::UIPanelSystem(Scene* scene) : System(scene)
	{
		MeshInfo quad_info = MeshInfo{&VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED, 1, MESH_FLAG_NONE};

		anchor_meshes[PIVOT_TOP_LEFT] = Resources::createMesh(quad_info);
		anchor_meshes[PIVOT_TOP_CENTER] = Resources::createMesh(quad_info);
		anchor_meshes[PIVOT_TOP_RIGHT] = Resources::createMesh(quad_info);
		anchor_meshes[PIVOT_CENTER_LEFT] = Resources::createMesh(quad_info);
		anchor_meshes[PIVOT_CENTER] = Resources::createMesh(quad_info);
		anchor_meshes[PIVOT_CENTER_RIGHT] = Resources::createMesh(quad_info);
		anchor_meshes[PIVOT_BOTTOM_LEFT] = Resources::createMesh(quad_info);
		anchor_meshes[PIVOT_BOTTOM_CENTER] = Resources::createMesh(quad_info);
		anchor_meshes[PIVOT_BOTTOM_RIGHT] = Resources::createMesh(quad_info);

		Geometry::createQuad(*anchor_meshes[PIVOT_TOP_LEFT], 1, 1, VERTEX_FLAG_UV, PIVOT_TOP_LEFT);
		Geometry::createQuad(*anchor_meshes[PIVOT_TOP_CENTER], 1, 1, VERTEX_FLAG_UV, PIVOT_TOP_CENTER);
		Geometry::createQuad(*anchor_meshes[PIVOT_TOP_RIGHT], 1, 1, VERTEX_FLAG_UV, PIVOT_TOP_RIGHT);
		Geometry::createQuad(*anchor_meshes[PIVOT_CENTER_LEFT], 1, 1, VERTEX_FLAG_UV, PIVOT_CENTER_LEFT);
		Geometry::createQuad(*anchor_meshes[PIVOT_CENTER], 1, 1, VERTEX_FLAG_UV, PIVOT_CENTER);
		Geometry::createQuad(*anchor_meshes[PIVOT_CENTER_RIGHT], 1, 1, VERTEX_FLAG_UV, PIVOT_CENTER_RIGHT);
		Geometry::createQuad(*anchor_meshes[PIVOT_BOTTOM_LEFT], 1, 1, VERTEX_FLAG_UV, PIVOT_BOTTOM_LEFT);
		Geometry::createQuad(*anchor_meshes[PIVOT_BOTTOM_CENTER], 1, 1, VERTEX_FLAG_UV, PIVOT_BOTTOM_CENTER);
		Geometry::createQuad(*anchor_meshes[PIVOT_BOTTOM_RIGHT], 1, 1, VERTEX_FLAG_UV, PIVOT_BOTTOM_RIGHT);

		scene->onDraw.subscribe(this, &UIPanelSystem::draw);
	}

	UIPanelSystem::~UIPanelSystem()
	{
		for (int i = 0; i < 9; i++)
		{
			delete anchor_meshes[i];
		}
	}


	void UIPanelSystem::draw(RenderGraph* graph)
	{
		Group<Node3D, Transform, UIPanel> group = scene->group<Node3D, Transform, UIPanel>();
		for (auto [entity_handle,node, transform, panel] : group)
		{
			if (!panel.active || !panel.pipeline_instance)
			{
				return;
			}
			DrawCmdInfo cmd{};
			cmd.mesh = anchor_meshes[panel.anchor];
			cmd.pipeline_instance = panel.pipeline_instance;
			cmd.order_in_layer = node.getGlobalIndex();
			PanelPushConstant push_constant{};
			push_constant.size = panel.size;
			push_constant.world_matrix = transform.world();


			PushConstantInfo push_constant_info{};
			push_constant_info.name = "constants";
			push_constant_info.size = sizeof(mat4) + sizeof(vec2);
			push_constant_info.data = &push_constant;
			cmd.push_constants = &push_constant_info;
			cmd.push_constants_count = 1;
			cmd.flags = DRAW_CMD_FLAG_ORDERED;
			cmd.layer = panel.layer;

			graph->add(cmd);
		}
	}
}
