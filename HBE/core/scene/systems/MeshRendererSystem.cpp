
#include "MeshRendererSystem.h"
#include "core/graphics/Graphics.h"
#include "core/utility/Profiler.h"
#include <core/scene/Scene.h>
#include <core/graphics/RenderGraph.h>

namespace HBE {
    MeshRendererSystem::MeshRendererSystem(Scene *scene) : System(scene) {
        scene->onDraw.subscribe(this, &MeshRendererSystem::draw);
        mesh_renderer_component_id = scene->getComponentTypeID<MeshRenderer>();
        transform_component_id = scene->getComponentTypeID<Transform>();
    }

    void MeshRendererSystem::drawNode(SceneNode &node, RenderGraph *render_graph) {
        DrawCmdInfo draw_cmd{};
        draw_cmd.flags = DRAW_CMD_FLAG_ORDERED;
        if (node.entity.has(mesh_renderer_component_id)) {
            MeshRenderer &mesh_renderer = node.entity.get<MeshRenderer>(mesh_renderer_component_id);
            if (mesh_renderer.active && mesh_renderer.ordered) {
                Transform &transform = node.entity.get<Transform>(transform_component_id);
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
                } else
                    Log::warning("Mesh renderer does not have a material and/or a mesh assigned");
            }
        }
        for (auto &child: node.children) {
            drawNode(child, render_graph);
        }
    }

    void HBE::MeshRendererSystem::draw(RenderGraph *render_graph) {
        HB_PROFILE_BEGIN("MeshRendererDraw");
		HB_PROFILE_BEGIN("MeshRendererUpdateUnordered");
        DrawCmdInfo draw_cmd{};

        auto group = scene->group<Transform, MeshRenderer>();

        for (auto[handle, transform, mesh_renderer]: group) {
            if (mesh_renderer.active && !mesh_renderer.ordered) {
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
                } else
                    Log::warning("Mesh renderer does not have a material and/or a mesh assigned");
            }
        }
        HB_PROFILE_END();

        HB_PROFILE_BEGIN("MeshRendererUpdateOrdered");
        std::list<SceneNode> nodes = scene->getSceneNodes();

        for (SceneNode node: nodes) {
            drawNode(node,render_graph);
        }
        HB_PROFILE_END();
    }

    MeshRendererSystem::~MeshRendererSystem() {
        scene->onDraw.unsubscribe(this);
    }
}


