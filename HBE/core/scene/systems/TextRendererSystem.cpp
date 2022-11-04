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
		auto group = scene->group<Transform, TextRenderer>();
		for (auto[handle, transform, text_renderer]:group) {
			if (text_renderer.active) {
				if (text_renderer.font && text_renderer.mesh && text_renderer.pipeline_instance) {
					text_renderer.pipeline_instance->setTexture("mtsdf", text_renderer.font->getTextureAtlas(), Graphics::getCurrentFrame());
					Graphics::draw(transform.world(), *text_renderer.mesh, *text_renderer.pipeline_instance);
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

