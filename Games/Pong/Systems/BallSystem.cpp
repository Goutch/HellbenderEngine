//
// Created by User on 2/16/2023.
//

#include "BallSystem.h"
#include "PaddleSystem.h"
#include "Games/Pong/PongGame.h"

namespace Pong {
	BallSystem::BallSystem(PongGameScene *scene, PongGameState &game_state, AudioClipInstance *bounce_sound, RenderTarget *render_target) : System(scene) {
		this->game_scene = scene;
		this->game_state = &game_state;
		this->bounce_sound = bounce_sound;
		scene->onUpdate.subscribe(this, &BallSystem::update);
		scene->onDraw.subscribe(this, &BallSystem::draw);
		scene->onAttach<BallComponent>().subscribe(this, &BallSystem::onAttachBallComponent);
		scene->onDetach<BallComponent>().subscribe(this, &BallSystem::onDetachBallComponent);

		VertexAttributeInfo vertex_attribute_infos[2]{
				VertexAttributeInfo{0, sizeof(vec3)+sizeof(vec2), VERTEX_ATTRIBUTE_FLAG_NONE},
				VertexAttributeInfo{1, sizeof(mat4), VERTEX_ATTRIBUTE_FLAG_PER_INSTANCE}
		};

		MeshInfo mesh_info{};
		mesh_info.flags = MESH_FLAG_NONE;
		mesh_info.attribute_infos = vertex_attribute_infos;
		mesh_info.attribute_info_count = 2;
		ball_mesh = Resources::createMesh(mesh_info);

		Geometry::createQuad(*ball_mesh, 1, 1, VERTEX_FLAG_UV);

		ShaderInfo shader_info{};
		shader_info.path = "shaders/defaults/InstancedPositionUV.vert";
		shader_info.stage = SHADER_STAGE_VERTEX;
		ball_vertex_shader = Resources::createShader(shader_info);

		shader_info.path = "shaders/defaults/InstancedPositionUVCircle.frag";
		shader_info.stage = SHADER_STAGE_FRAGMENT;
		ball_fragment_shader = Resources::createShader(shader_info);

		GraphicPipelineInfo pipeline_info{};
		pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_CULL_BACK;
		pipeline_info.vertex_shader = ball_vertex_shader;
		pipeline_info.fragment_shader = ball_fragment_shader;
		pipeline_info.attribute_infos = vertex_attribute_infos;
		pipeline_info.attribute_info_count = 2;
		pipeline_info.render_target = render_target;

		ball_pipeline = Resources::createGraphicPipeline(pipeline_info);

		GraphicPipelineInstanceInfo pipeline_instance_info{};
		pipeline_instance_info.graphic_pipeline = ball_pipeline;
		pipeline_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;
		ball_pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info);

		vec4 color = {0xfa/255.0f, 0xbd/255.0f, 0x2f/255.0f, 1};
		ball_pipeline_instance->setUniform("material", &color);
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


	bool paused = false;

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

		if (Input::getKeyDown(KEY_P)) {
			paused = !paused;
		}
		if (paused) {
			return;
		}
		auto paddle_group = scene->group<Transform, PaddleComponent>();
		std::vector<PaddleData> paddles;
		for (auto [entity, transform, paddle]: paddle_group) {
			paddles.push_back({&transform, &paddle});
		}
		uint32_t n = 0;
		auto group = scene->group<Transform, BallComponent>();
		for (auto [entity, transform, ball]: group) {
			bool colide = false;
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
			transform.setPosition(vec3(new_pos, 0.0f));


			colide = colide | collideBallWithPaddles(paddles, &transform, &ball);


			if (colide && bounce_sound->getState() != AUDIO_CLIP_INSTANCE_STATE_PLAYING) {
				bounce_sound->setVolume(glm::clamp(t_since_last_bounce, 0.002f, 1.0f));
				bounce_sound->updatePostion(transform.worldPosition(), vec3(0, 0, 0));
				bounce_sound->play();

				t_since_last_bounce = 0;
			}

			ball_transforms[n] = transform.world();
			n++;
		}


		t += delta;
		t_since_last_bounce += delta;
		if (t >= 1) {
			t = 0;
			Log::debug("#balls:" + std::to_string(n));
		}
	}

	void BallSystem::draw(RenderGraph *render_graph) {
		if (!paused)
			ball_mesh->setInstanceBuffer(1, ball_transforms.data(), ball_transforms.size());

		DrawCmdInfo draw_cmd_info{};
		draw_cmd_info.mesh = ball_mesh;
		draw_cmd_info.pipeline_instance = ball_pipeline_instance;
		draw_cmd_info.flags = DRAW_CMD_FLAG_NONE;

		render_graph->draw(draw_cmd_info);
	}

	BallSystem::~BallSystem() {
		delete ball_mesh;
		delete ball_vertex_shader;
		delete ball_fragment_shader;
		delete ball_pipeline_instance;
		delete ball_pipeline;
	}

	void BallSystem::onAttachBallComponent(Entity entity) {
		ball_transforms.emplace_back(1.0f);
	}

	void BallSystem::onDetachBallComponent(Entity entity) {
		ball_transforms.pop_back();
	}
}

