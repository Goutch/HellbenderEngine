//
// Created by User on 2/18/2023.
//

#include "PaddleSystem.h"
namespace Pong {
	void PaddleSystem::update(float delta) {
		auto group = scene->group<Transform, PaddleComponent>();

		for (auto [entity, transform, paddle]: group) {
			vec3 position = transform.position();
			vec3 new_position = position;
			if (Input::getKey(paddle.up_key)) {
				new_position.y += paddle.speed * delta;
			}
			if (Input::getKey(paddle.down_key)) {
				new_position.y -= paddle.speed * delta;
			}

			Area game_area = this->game_scene->getArea();
			if (new_position.y - (paddle.size_y / 2.0f) < game_area.position.y) {
				new_position.y = game_area.position.y + (paddle.size_y / 2.0f);
			}
			if (new_position.y + (paddle.size_y / 2.0f) > game_area.position.y + game_area.size.y) {
				new_position.y = game_area.position.y + game_area.size.y - (paddle.size_y / 2.0f);
			}
			transform.setPosition(new_position);
			transform.setLocalScale(vec3(paddle.size_x, paddle.size_y, 1));
		}
	}

	PaddleSystem::PaddleSystem(PongGameScene *game_scene) : System(game_scene) {
		game_scene->onUpdate.subscribe(this, &PaddleSystem::update);
		this->game_scene = game_scene;
	}

	PaddleSystem::~PaddleSystem() {
		game_scene->onUpdate.unsubscribe(this);
	}
}
