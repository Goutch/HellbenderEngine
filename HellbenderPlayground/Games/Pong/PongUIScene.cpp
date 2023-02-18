//
// Created by User on 2/12/2023.
//

#include "PongUIScene.h"

namespace Pong {

	void PongUIScene::OnWindowSizeChange(Window *window) {
		render_target->setResolution(window->getWidth(), window->getHeight());
	}

	PongUIScene::PongUIScene() {

		RenderTargetInfo render_target_info{};
		render_target_info.clear_color = vec4(0, 0, 0, 0);
		render_target_info.width = Graphics::getWindow()->getWidth();
		render_target_info.height = Graphics::getWindow()->getHeight();
		render_target_info.flags = RENDER_TARGET_FLAG_CLEAR_COLOR | RENDER_TARGET_FLAG_COLOR_ATTACHMENT;
		render_target = Resources::createRenderTarget(render_target_info);

		Entity camera_entity = createEntity3D();
		PixelCamera &camera = camera_entity.attach<PixelCamera>();
		camera.setRenderTarget(render_target);

		Graphics::getWindow()->onSizeChange.subscribe(this, &PongUIScene::OnWindowSizeChange);

		fps_counter = new FPSCounter(*this, render_target);
	}

	PongUIScene::~PongUIScene() {
		delete fps_counter;
		delete render_target;
	}
}