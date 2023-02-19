//
// Created by User on 2/16/2023.
//

#include "BallSystem.h"
#include "PaddleSystem.h"
#include "Games/Pong/PongGame.h"

namespace Pong {

	BallSystem::BallSystem(PongGameScene *scene, PongGameState &game_state) : System(scene) {
		this->game_scene = scene;
		this->game_state = &game_state;
		scene->onUpdate.subscribe(this, &BallSystem::update);
	}

	struct PaddleData {
		Transform *transform;
		PaddleComponent *paddle;
	};

	float sdBox(vec2 point, vec2 box_pos, vec2 box_half_size) {
		vec2 p = point - box_pos;
		vec2 q = abs(p) - box_half_size;
		return glm::length(glm::max(q, vec2(0.0f))) + glm::min(glm::max(q.x, q.y), 0.0f);
	}

	bool collideBallWithPaddles(std::vector<PaddleData> &paddles, Transform *transform, BallComponent *ball) {

		for (PaddleData &paddle_data: paddles) {
			Transform *paddle_transform = paddle_data.transform;
			PaddleComponent *paddle = paddle_data.paddle;

			vec2 ball_pos = transform->worldPosition();
			vec2 paddle_pos = paddle_transform->worldPosition();


			float distance = sdBox(ball_pos, paddle_pos, vec2(paddle->size_x / 2.0f, paddle->size_y / 2.0f));
			distance -= ball->radius;
			if (distance < 0) {
				if (ball_pos.x < paddle_pos.x) {
					ball->velocity.x = -abs(ball->velocity.x);
				} else {
					ball->velocity.x = abs(ball->velocity.x);
				}
				return true;
			}
		}
		return false;
	}


	void BallSystem::update(float delta) {
		bool delete_ball = false;
		vec2 delete_pos = vec2(0, 0);
		float delete_radius = 1;
		if (Input::getKey(KEY_MOUSE_BUTTON_LEFT)) {
			vec2 create_pos = Input::getNormalizedMousePosition();
			create_pos -= vec2(0.5f, 0.5f);
			create_pos.y *= -1;
			Camera2D &camera = game_scene->getCameraEntity().get<Camera2D>();
			create_pos.x *= camera.aspectRatio();
			create_pos.x *= camera.getZoomRatio();
			create_pos.y *= camera.getZoomRatio();
			game_scene->createBall(create_pos, vec2(Random::floatRange(-10, 10), Random::floatRange(-10, 10)));
		}
		if (Input::getKey(KEY_MOUSE_BUTTON_RIGHT)) {
			delete_ball = true;
			delete_pos = Input::getNormalizedMousePosition();
			delete_pos -= vec2(0.5f, 0.5f);
			delete_pos.y *= -1;
			Camera2D &camera = game_scene->getCameraEntity().get<Camera2D>();
			delete_pos.x *= camera.aspectRatio();
			delete_pos.x *= camera.getZoomRatio();
			delete_pos.y *= camera.getZoomRatio();
		}


		auto paddle_group = scene->group<Transform, PaddleComponent>();
		std::vector<PaddleData> paddles;
		for (auto [entity, transform, paddle]: paddle_group) {
			paddles.push_back({&transform, &paddle});
		}
		uint32_t n = 0;
		auto group = scene->group<Transform, BallComponent>();
		for (auto [entity, transform, ball]: group) {

			if (delete_ball) {
				if (glm::distance(vec2(transform.worldPosition()), delete_pos) < delete_radius) {
					scene->destroyEntity(entity);
					continue;
				}
			}

			//check for collision with walls
			Area area = game_scene->getArea();
			vec2 pos = transform.worldPosition();
			vec2 new_pos = pos + (ball.velocity * delta);


			if (new_pos.x + ball.radius > area.position.x + area.size.x) {
				//collide with wall right
				ball.velocity.x = -ball.velocity.x;
				new_pos.x = area.position.x + area.size.x - ball.radius;
				game_state->score_left++;
			} else if (new_pos.x - ball.radius < area.position.x) {
				//collide with wall left
				ball.velocity.x = -ball.velocity.x;
				new_pos.x = area.position.x + ball.radius;
				game_state->score_right++;
			}
			if (new_pos.y - ball.radius < area.position.y) {
				ball.velocity.y = -ball.velocity.y;
				new_pos.y = area.position.y + ball.radius;
			} else if (new_pos.y + ball.radius > area.position.y + area.size.y) {
				ball.velocity.y = -ball.velocity.y;
				new_pos.y = area.position.y + area.size.y - ball.radius;
			}
			transform.setPosition(vec3(new_pos, 0.0f));


			collideBallWithPaddles(paddles, &transform, &ball);
			n++;
		}


		t += delta;
		if (t >= 1) {
			t = 0;
			Log::debug("#balls:" + std::to_string(n));
		}
	}

}

