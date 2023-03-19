#include "BallSystem.h"
#include "PaddleSystem.h"
#include "PongGame.h"

namespace PongLockstep {

	struct PaddleData {
		PaddleComponent *paddle;
	};

	BallSystem::BallSystem(PongGameScene *scene, PongGameState &game_state, AudioClipInstance *bounce_sound) : System(scene) {
		this->game_scene = scene;
		this->game_state = &game_state;
		this->bounce_sound = bounce_sound;
		scene->onUpdate.subscribe(this, &BallSystem::update);
		game_state.onStep.subscribe(this, &BallSystem::step);
	}


	BallSystem::~BallSystem() {
		game_scene->onUpdate.unsubscribe(this);
		game_state->onStep.unsubscribe(this);
	}

	fix16 sdBox(vec2fix16 point, vec2fix16 box_pos, vec2fix16 box_half_size) {
		vec2fix16 p = point - box_pos;
		vec2fix16 q = vec2fix16(abs(p.x), abs(p.y)) - box_half_size;

		return maxVec2Fix(q, vec2fix16(0, 0)).magnitude() + min(max(q.x, q.y), fix16(0));
	}

	bool collideBallWithPaddles(std::vector<PaddleComponent *> &paddles, vec2fix16 ball_pos, BallComponent *ball) {
		for (PaddleComponent *paddle: paddles) {
			vec2fix16 paddle_pos = paddle->position;


			fix16 distance = sdBox(ball_pos, paddle_pos, vec2fix16(paddle->size_x / fix16(2), paddle->size_y / fix16(2)));
			distance -= ball->radius;
			if (distance < fix16(0)) {
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
	void BallSystem::step(StepData step, StepInputsData *inputs) {
		auto paddle_group = scene->group<Transform, PaddleComponent>();
		std::vector<PaddleComponent *> paddles;
		for (auto [entity, transform, paddle]: paddle_group) {
			paddles.push_back({&paddle});
		}
		uint32_t n = 0;
		auto group = scene->group<BallComponent>();
		for (auto [entity, ball]: group) {
			bool colide = false;

			//check for collision with walls
			Area area = game_scene->getArea();
			vec2fix16 new_pos = ball.position + (ball.velocity * step.delta);

			if (new_pos.x + ball.radius > area.position.x + area.size.x) {
				//collide with wall right
				ball.velocity.x = -ball.velocity.x;
				new_pos.x = area.position.x + area.size.x - ball.radius;
				game_state->score_left++;
				colide = true;
			} else if (new_pos.x - ball.radius < area.position.x) {
				//collide with wall left
				ball.velocity.x = -ball.velocity.x;
				new_pos.x = area.position.x + ball.radius;
				game_state->score_right++;
				colide = true;
			}
			if (new_pos.y - ball.radius < area.position.y) {
				ball.velocity.y = -ball.velocity.y;
				new_pos.y = area.position.y + ball.radius;
				colide = true;
			} else if (new_pos.y + ball.radius > area.position.y + area.size.y) {
				ball.velocity.y = -ball.velocity.y;
				new_pos.y = area.position.y + area.size.y - ball.radius;
				colide = true;
			}

			ball.position = new_pos;

			colide = colide | collideBallWithPaddles(paddles, ball.position, &ball);
			n++;

			if (colide && bounce_sound->getState() != AUDIO_CLIP_INSTANCE_STATE_PLAYING) {
				bounce_sound->setVolume(glm::clamp(t_since_last_bounce, 0.002f, 1.0f));
				bounce_sound->updatePostion(vec3(ball.position.toVec2(), 0), vec3(0, 0, 0));
				bounce_sound->play();

				t_since_last_bounce = 0;
			}
		}


		t += step.delta;
		t_since_last_bounce += static_cast<float>(step.delta);
		if (t >= fix16(1)) {
			t = fix16(0);
			Log::debug("#balls:" + std::to_string(n));
		}
	}


	void BallSystem::update(float delta) {
		auto group = scene->group<BallComponent, Transform>();
		for (auto [entity, ball, transform]: group) {
			transform.setPosition(vec3(static_cast<float>(ball.position.x), static_cast<float>(ball.position.y), 0.0f));
		}
	}

}

