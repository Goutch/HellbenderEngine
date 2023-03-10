
#include "PongGame.h"

namespace PongLockstep {
	const vec4 PongGame::LEFT_COLOR = {104.0f / 255.0f, 157.0f / 255.0f, 106.0f / 255.0f, 1};
	const vec4 PongGame::RIGHT_COLOR = {204.0f / 255.0f, 36.0f / 255.0f, 29.0f / 255.0f, 1};

	void reset(PongGameState &game_state) {
		game_state.score_left = 0;
		game_state.score_right = 0;
	}

	PongGame::PongGame() {
		Application::onPresent.subscribe(this, &PongGame::onPresent);
		reset(game_state);
		ui_scene = new PongUIScene(game_state);
		game_scene = new PongGameScene(game_state);
	}

	PongGame::~PongGame() {
		Application::onPresent.unsubscribe(this);
		delete ui_scene;
		delete game_scene;
	}

	void PongGame::onPresent() {
		std::vector<Texture *> present_images;
		if (game_scene->isActive() && game_scene->getMainCameraTexture() != nullptr) {
			present_images.push_back(game_scene->getMainCameraTexture());
		}
		if (game_scene->isActive() && ui_scene->getMainCameraTexture() != nullptr) {
			present_images.push_back(ui_scene->getMainCameraTexture());
		}
		PresentCmdInfo present_info{};
		present_info.image_count = present_images.size();
		present_info.images = present_images.data();
		Graphics::present(present_info);
	}

}
