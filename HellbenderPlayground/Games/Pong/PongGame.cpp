
#include "PongGame.h"

namespace Pong {


	PongGame::PongGame() {
		Application::onPresent.subscribe(this, &Pong::PongGame::onPresent);
	}
	PongGame::~PongGame() {
		Application::onPresent.unsubscribe(this);
	}
	void PongGame::onPresent() {
		std::vector<Texture *> present_images;
		if (game_scene.isActive() && game_scene.getMainCameraTexture() != nullptr) {
			present_images.push_back(game_scene.getMainCameraTexture());
		}
		if (game_scene.isActive() && ui_scene.getMainCameraTexture() != nullptr) {
			present_images.push_back(ui_scene.getMainCameraTexture());
		}
		PresentCmdInfo present_info{};
		present_info.image_count = present_images.size();
		present_info.images = present_images.data();
		Graphics::present(present_info);
	}

}
