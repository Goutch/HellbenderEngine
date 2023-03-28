
#include "PongGame.h"

namespace PongLockstep {
	const vec4 PongGame::LEFT_COLOR = {104.0f / 255.0f, 157.0f / 255.0f, 106.0f / 255.0f, 1};
	const vec4 PongGame::RIGHT_COLOR = {204.0f / 255.0f, 36.0f / 255.0f, 29.0f / 255.0f, 1};

	void PongGame::reset(PongGameState &game_state, const std::string &ip) {
		game_state.score_left = 0;
		game_state.score_right = 0;
		if (game_state.client != nullptr) {
			game_state.client->on_receive_event.unsubscribe(this);
			delete game_state.client;
		}
		ClientInfo client_info{};
		client_info.server_ip = ip;
		client_info.server_port = 25565;
		client_info.max_channels = 2;
		game_state.client = new Client(client_info);
		game_state.client->on_receive_event.subscribe(this, &PongGame::onEventReceived);
		if (!game_state.client->connect(1000)) {
			Log::error("Failed to connect to server");
		}
	}

	PongGame::PongGame(const std::string &ip) {
		Application::onPresent.subscribe(this, &PongGame::onPresent);
		Application::onUpdate.subscribe(this, &PongGame::onUpdate);
		reset(game_state, ip);
		ui_scene = new PongUIScene(game_state);
		game_scene = new PongGameScene(game_state);
	}

	PongGame::~PongGame() {
		Application::onPresent.unsubscribe(this);
		delete game_state.client;
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


	void PongGame::onEventReceived(void *data) {
		MESSAGE_TYPE type = *(MESSAGE_TYPE *) data;
		switch (type) {
			case MESSAGE_TYPE_STEP:
				step_queue.push(*(StepData *) (data));
				break;
		}
	}

	void PongGame::step(StepData &step_data) {
		//Log::message(std::to_string((Application::getTime() - last_step_time) * SECONDS_TO_MILISECONDS) + "ms");
		StepInputsData stepInputsData{};
		stepInputsData.frame_id = step_data.frame_id + STEP_OFFSET;
		game_state.onStep.invoke(step_data, &stepInputsData);

		PacketInfo packet_info{};
		packet_info.data = &stepInputsData;
		packet_info.length = sizeof(StepInputsData);
		packet_info.mode = PACKET_MODE_RELIABLE_ORDERED;
		packet_info.channel = 0;

		game_state.client->send(packet_info);

		last_step_time = Application::getTime();
	}

	void PongGame::onUpdate(float delta) {
		game_state.client->pollEvents();
		if (
			Application::getTime() - last_step_time >= 1.0f / static_cast<float>(STEP_PER_SECONDS) &&
			!step_queue.empty()) {
			step(step_queue.front());
			step_queue.pop();
		}
	}

}
