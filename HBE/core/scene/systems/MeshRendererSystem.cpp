
#include "MeshRendererSystem.h"
#include "core/graphics/Graphics.h"
#include "core/utility/Profiler.h"
#include <core/scene/Scene.h>

namespace HBE {
	MeshRendererSystem::MeshRendererSystem(Scene *scene) : System(scene) {
		scene->onDraw.subscribe(this, &MeshRendererSystem::draw);
	}

	void HBE::MeshRendererSystem::draw() {
		Profiler::begin("MeshRendererUpdate");
		DrawCmdInfo draw_cmd{};


		Profiler::begin("MeshRendererUpdateGroup");
		auto group = scene->group<Transform, MeshRenderer>();
		Profiler::end();
		for (auto [handle, transform, mesh_renderer]: group) {
			if (mesh_renderer.active) {
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
					Graphics::draw(draw_cmd);

				} else
					Log::warning("Mesh renderer does not have a material and/or a mesh assigned");
			}

		}

		Profiler::end();
	}

	MeshRendererSystem::~MeshRendererSystem() {
		scene->onDraw.unsubscribe(this);
	}
}


