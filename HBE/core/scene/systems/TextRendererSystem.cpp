//
// Created by user on 11/4/2022.
//

#include <core/graphics/Graphics.h>
#include "TextRendererSystem.h"
#include "core/resource/GraphicPipelineInstance.h"
#include "core/resource/Mesh.h"

namespace HBE {
	TextRendererSystem::TextRendererSystem(HBE::Scene *scene) : System(scene) {
		scene->onDraw.subscribe(this, &TextRendererSystem::draw);
		scene->onDetach<TextRenderer>().subscribe(this, &TextRendererSystem::onDetachTextRenderer);
	}

	TextRendererSystem::~TextRendererSystem() {
		scene->onDraw.unsubscribe(this);
	}

	void TextRendererSystem::draw() {

		DrawCmdInfo draw_cmd{};
		PushConstantInfo push_constant_info{};
		push_constant_info.size = sizeof(mat4);
		push_constant_info.name = "constants";
		draw_cmd.push_constants_count = 1;
		draw_cmd.push_constants = &push_constant_info;
		//draw_cmd.flags = DRAW_CMD_FLAG_ORDERED;

		auto group = scene->group<Transform, TextRenderer>();
		for (auto[handle, transform, text_renderer]:group) {
			if (text_renderer.active) {
				if (text_renderer.font && text_renderer.mesh && text_renderer.pipeline_instance) {
					text_renderer.pipeline_instance->setTexture("mtsdf", text_renderer.font->getTextureAtlas(), Graphics::getCurrentFrame());

					draw_cmd.mesh = text_renderer.mesh;
					draw_cmd.pipeline_instance = text_renderer.pipeline_instance;

					mat4 world_matrix = transform.world();
					push_constant_info.data = &world_matrix;

					Graphics::draw(draw_cmd);
				} else
					Log::warning("Text renderer does not have a pipeline and/or a font assigned and/or a mesh assigned");
			}
		}

	}

	void TextRendererSystem::onDetachTextRenderer(Entity entity) {
		auto &text_renderer = entity.get<TextRenderer>();
		if (text_renderer.mesh != nullptr)
			delete text_renderer.mesh;
	}
}

