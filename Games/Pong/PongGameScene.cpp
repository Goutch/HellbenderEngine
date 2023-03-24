
#include "PongGameScene.h"
#include "Games/Pong/Systems/BallSystem.h"
#include "Games/Pong/Systems/PaddleSystem.h"
#include "PongGame.h"


namespace Pong {

	Area &PongGameScene::getArea() {
		return game_area;
	}

	PongGameScene::PongGameScene(PongGameState &game_state) : Scene() {
		createResources();
		setupScene();
		Graphics::getWindow()->onSizeChange.subscribe(this, &PongGameScene::OnWindowSizeChange);

		addSystem(new BallSystem(this, game_state, bounce_sound_instance));
		addSystem(new PaddleSystem(this));

	}

	PongGameScene::~PongGameScene() {

		render_target->onResolutionChange.unsubscribe(this);
		delete quad_mesh;
		delete vertex_shader;
		delete fragment_shader;
		delete pipeline;
		delete ball_pipeline_instance;
		delete paddle_left_pipeline_instance;
		delete paddle_right_pipeline_instance;
		delete render_target;
		delete bounce_sound_instance;
		delete bounce_sound;
	}

	Entity PongGameScene::createBall(vec2 position, vec2 velocity) {
		Entity ball = createEntity3D();
		MeshRenderer &ball_renderer = ball.attach<MeshRenderer>();
		ball_renderer.mesh = quad_mesh;
		ball_renderer.layer = 0;
		ball_renderer.pipeline_instance = ball_pipeline_instance;

		BallComponent &ballComponent = ball.attach<BallComponent>();
		ballComponent.velocity = velocity;
		ballComponent.radius = 0.2;
		ball.get<Transform>().setPosition(vec3(position, 0));
		ball.get<Transform>().setLocalScale(vec3(ballComponent.radius));

		return ball;
	}

	Entity PongGameScene::createPaddle(vec3 position, KEY up_key, KEY down_key, GraphicPipelineInstance *paddle_pipeline_instance) {
		Entity paddle = createEntity3D();
		MeshRenderer &paddle_renderer = paddle.attach<MeshRenderer>();
		paddle_renderer.mesh = quad_mesh;
		paddle_renderer.layer = 0;
		paddle_renderer.pipeline_instance = paddle_pipeline_instance;

		PaddleComponent &paddleComponent = paddle.attach<PaddleComponent>();
		paddleComponent.speed = 10;
		paddleComponent.size_x = 0.3;
		paddleComponent.size_y = 2.0;
		paddleComponent.down_key = down_key;
		paddleComponent.up_key = up_key;

		paddle.get<Transform>().setPosition(position);

		return paddle;
	}

	void PongGameScene::setupScene() {
		Entity camera_entity = createEntity3D();
		Camera2D &camera = camera_entity.attach<Camera2D>();
		camera.setRenderTarget(render_target);

		createBall(vec2(0, 0), vec2(10, 10));
		paddle_left_entity = createPaddle(vec3{-game_area.size.x / 2 + 1, 0, 0},
										  KEY_W,
										  KEY_S,
										  paddle_left_pipeline_instance);
		paddle_right_entity = createPaddle(vec3{game_area.size.x / 2 - 1, 0, 0},
										   KEY_UP,
										   KEY_DOWN,
										   paddle_right_pipeline_instance);
		render_target->onResolutionChange.subscribe(this, &PongGameScene::onRenderTargetResolutionChange);
		onRenderTargetResolutionChange(render_target);
	}

	void PongGameScene::createResources() {
		RenderTargetInfo render_target_info{};
		render_target_info.width = Graphics::getWindow()->getWidth();
		render_target_info.height = Graphics::getWindow()->getHeight();
		render_target_info.flags = RENDER_TARGET_FLAG_COLOR_ATTACHMENT | RENDER_TARGET_FLAG_CLEAR_COLOR;
		render_target = Resources::createRenderTarget(render_target_info);

		MeshInfo mesh_info{};
		mesh_info.attribute_info_count = 1;
		mesh_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
		quad_mesh = Resources::createMesh(mesh_info);
		Geometry::createQuad(*quad_mesh, 1, 1, VERTEX_FLAG_NONE);

		ShaderInfo vertex_shader_info{};
		ShaderInfo fragment_shader_info{};

		vertex_shader_info.path = "shaders/defaults/Position.vert";
		vertex_shader_info.stage = SHADER_STAGE_VERTEX;

		fragment_shader_info.path = "shaders/defaults/Position.frag";
		fragment_shader_info.stage = SHADER_STAGE_FRAGMENT;

		vertex_shader = Resources::createShader(vertex_shader_info);
		fragment_shader = Resources::createShader(fragment_shader_info);

		GraphicPipelineInfo pipeline_info{};
		pipeline_info.attribute_info_count = 1;
		pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
		pipeline_info.vertex_shader = vertex_shader;
		pipeline_info.fragement_shader = fragment_shader;
		pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_NONE;
		pipeline_info.render_target = render_target;
		pipeline = Resources::createGraphicPipeline(pipeline_info);

		GraphicPipelineInstanceInfo pipeline_instance_info{};
		pipeline_instance_info.graphic_pipeline = pipeline;
		pipeline_instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;

		ball_pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info);
		paddle_left_pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info);
		paddle_right_pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info);

		vec4 color = {1, 1, 1, 1};
		ball_pipeline_instance->setUniform("material", &color);
		color = PongGame::LEFT_COLOR;
		paddle_left_pipeline_instance->setUniform("material", &color);
		color = PongGame::RIGHT_COLOR;
		paddle_right_pipeline_instance->setUniform("material", &color);


		AudioClipInfo audio_clip_info{};
		audio_clip_info.path = "sounds/8BitHit.wav";
		AudioClipInstanceInfo audio_clip_instance_info{};
		bounce_sound = Resources::createAudioClip(audio_clip_info);

		audio_clip_instance_info.clip = bounce_sound;
		audio_clip_instance_info.volume = 0.1;
		audio_clip_instance_info.pitch = 1.0f;
		bounce_sound_instance = Resources::createAudioClipInstance(audio_clip_instance_info);
	}

	void PongGameScene::OnWindowSizeChange(Window *window) {
		render_target->setResolution(window->getWidth(), window->getHeight());
	}

	void PongGameScene::onRenderTargetResolutionChange(RenderTarget *render_target) {
		Entity camera_entity = getCameraEntity();
		Camera2D &camera = camera_entity.get<Camera2D>();

		float height = camera.getZoomRatio();
		float width = height * camera.aspectRatio();

		game_area = Area{{-width / 2.0f, -height / 2.0f},
						 {width,         height}};

		vec3 paddle_left_position = paddle_left_entity.get<Transform>().position();
		vec3 paddle_right_position = paddle_right_entity.get<Transform>().position();
		paddle_left_position.x = -game_area.size.x / 2 + 1;
		paddle_right_position.x = game_area.size.x / 2 - 1;
		paddle_left_entity.get<Transform>().setPosition(paddle_left_position);
		paddle_right_entity.get<Transform>().setPosition(paddle_right_position);
	}
}