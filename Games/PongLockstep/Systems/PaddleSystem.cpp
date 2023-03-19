//
// Created by User on 2/18/2023.
//

#include "PaddleSystem.h"
#include "PongGame.h"

namespace PongLockstep {
	void PaddleSystem::update(float delta) {
		auto group = scene->group<Transform, PaddleComponent>();

		for (auto [entity, transform, paddle]: group) {
			vec3fix16 position = vec3fix16(paddle.position.x, paddle.position.y, fix16(0));
			transform.setPosition(position.toVec3());
			transform.setLocalScale(vec3(static_cast<float>(paddle.size_x), static_cast<float>(paddle.size_y), 1));
		}
	}

	PaddleSystem::PaddleSystem(PongGameScene *game_scene, PongGameState &game_state) : System(game_scene) {
		this->game_state = &game_state;
		game_scene->onUpdate.subscribe(this, &PaddleSystem::update);
		game_state.onStep.subscribe(this, &PaddleSystem::step);
		this->game_scene = game_scene;
	}

	PaddleSystem::~PaddleSystem() {
		game_scene->onUpdate.unsubscribe(this);
	}

	void PaddleSystem::step(StepData step,StepInputsData* step_input) {
		auto group = scene->group<PaddleComponent>();

		for (auto [entity, paddle]: group) {
			vec2fix16 position = paddle.position;
			vec2fix16 new_position = position;
			new_position.x += step.inputs[paddle.client_id].paddle_direction * paddle.speed * step.delta;

			if (paddle.client_id == game_state->client->getID()) {
				if (Input::getKey(paddle.up_key)) {
					new_position.y += paddle.speed * step.delta;
				}
				if (Input::getKey(paddle.down_key)) {
					new_position.y -= paddle.speed * step.delta;
				}
			}


			Area game_area = this->game_scene->getArea();
			if (new_position.y - (paddle.size_y / fix16(2)) < game_area.position.y) {
				new_position.y = game_area.position.y + (paddle.size_y / fix16(2));
			}
			if (new_position.y + (paddle.size_y / fix16(2)) > game_area.position.y + game_area.size.y) {
				new_position.y = game_area.position.y + game_area.size.y - (paddle.size_y / fix16(2));
			}
			paddle.position = new_position;
		}
	}
}
