//
// Created by User on 2/16/2023.
//

#include "BallSystem.h"

namespace Pong {

	BallSystem::BallSystem(PongGameScene *scene) : System(scene) {
		this->game_scene = scene;
		scene->onUpdate.subscribe(this, &BallSystem::update);
	}

	void BallSystem::update(float delta) {
		auto group = scene->group<Transform, BallComponent>();
		for (auto [entity, transform, ball]: group) {

			//check for collision with walls
			Area area = game_scene->getArea();
			vec2 pos = transform.worldPosition();
			vec2 new_pos = pos + (ball.velocity * delta);

			if (new_pos.x + ball.radius > area.position.x + area.size.x) {
				ball.velocity.x = -ball.velocity.x;
				new_pos.x = area.position.x + area.size.x - ball.radius;
			}
			else if (new_pos.x - ball.radius < area.position.x) {
				ball.velocity.x = -ball.velocity.x;
				new_pos.x = area.position.x + ball.radius;
			}
			if (new_pos.y - ball.radius < area.position.y) {
				ball.velocity.y = -ball.velocity.y;
				new_pos.y = area.position.y + ball.radius;
			}
			else if (new_pos.y + ball.radius > area.position.y + area.size.y) {
				ball.velocity.y = -ball.velocity.y;
				new_pos.y = area.position.y + area.size.y - ball.radius;
			}

			transform.setPosition(vec3(new_pos, 0.0f));
		}
	}

}

