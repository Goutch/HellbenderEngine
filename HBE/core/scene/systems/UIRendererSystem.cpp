
#include "UIRendererSystem.h"
#include <core/graphics/Graphics.h>
#include "core/resource/GraphicPipelineInstance.h"
#include "core/resource/Mesh.h"

namespace HBE {

	UIRendererSystem::UIRendererSystem(Scene *scene) : System(scene) {
		scene->onDraw.subscribe(this, &UIRendererSystem::draw);
	}

	UIRendererSystem::~UIRendererSystem() {
		scene->onDraw.unsubscribe(this);
	}

	void UIRendererSystem::draw() {
		/*DrawCmdInfo draw_cmd{};
		PushConstantInfo push_constant_info{};
		push_constant_info.size = sizeof(mat3);
		push_constant_info.name = "constants";
		draw_cmd.push_constants_count = 1;
		draw_cmd.push_constants = &push_constant_info;
		draw_cmd.flags = DRAW_CMD_FLAG_ORDERED;

		auto group = scene->group<Transform2D, UIRenderer>();
		for (auto[handle, transform, ui_renderer]:group) {
			if (ui_renderer.active) {
				if (ui_renderer.mesh && ui_renderer.pipeline_instance) {
					draw_cmd.mesh = ui_renderer.mesh;
					draw_cmd.pipeline_instance = ui_renderer.pipeline_instance;

					mat3 world_matrix = transform.world();
					push_constant_info.data = &world_matrix;

					Graphics::draw(draw_cmd);
				} else
					Log::warning("UI renderer does not have a pipeline and/or a font assigned and/or a mesh assigned");
			}
		}*/

	}


}
