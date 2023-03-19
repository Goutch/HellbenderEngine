
#include "PongGameScene.h"
#include "Systems/BallSystem.h"
#include "Systems/PaddleSystem.h"
#include "PongGame.h"


namespace PongLockstep {

	Area &PongGameScene::getArea() {
		return game_area;
	}

	PongGameScene::PongGameScene(PongGameState &game_state) {
		createResources();
		setupScene();
		Graphics::getWindow()->onSizeChange.subscribe(this, &PongGameScene::OnWindowSizeChange);

		addSystem(new BallSystem(this, game_state, bounce_sound_instance));
		addSystem(new PaddleSystem(this, game_state));

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

	Entity PongGameScene::createBall(vec2fix16 position, vec2fix16 velocity) {
		Entity ball = createEntity3D();
		MeshRenderer &ball_renderer = ball.attach<MeshRenderer>();
		ball_renderer.mesh = quad_mesh;
		ball_renderer.layer = 0;
		ball_renderer.pipeline_instance = ball_pipeline_instance;

		BallComponent &ballComponent = ball.attach<BallComponent>();
		ballComponent.velocity = velocity;
		ballComponent.radius = fix16(2) / fix16(10);

		ballComponent.position = position;
		ball.get<Transform>().setLocalScale(vec3(static_cast<float>(ballComponent.radius),
												 static_cast<float>(ballComponent.radius),
												 1));

		return ball;
	}

	Entity PongGameScene::createPaddle(vec3 position, KEY up_key, KEY down_key, GraphicPipelineInstance *paddle_pipeline_instance, uint32_t client_id) {
		Entity paddle = createEntity3D();
		MeshRenderer &paddle_renderer = paddle.attach<MeshRenderer>();
		paddle_renderer.mesh = quad_mesh;
		paddle_renderer.layer = 0;
		paddle_renderer.pipeline_instance = paddle_pipeline_instance;

		PaddleComponent &paddleComponent = paddle.attach<PaddleComponent>();
		paddleComponent.speed = fix16(10);
		paddleComponent.size_x = fix16(3) / 10;
		paddleComponent.size_y = fix16(2);
		paddleComponent.down_key = down_key;
		paddleComponent.up_key = up_key;
		paddleComponent.client_id = client_id;

		paddle.get<Transform>().setPosition(position);

		return paddle;
	}

	void PongGameScene::setupScene() {
		Entity camera_entity = createEntity3D();
		Camera2D &camera = camera_entity.attach<Camera2D>();
		camera.setRenderTarget(render_target);

		createBall(vec2fix16(0, 0), vec2fix16(10, 10));
		paddle_left_entity = createPaddle(vec3{-game_area.size.x / 2 + 1, 0, 0},
										  KEY_W,
										  KEY_S,
										  paddle_left_pipeline_instance,
										  0);
		paddle_right_entity = createPaddle(vec3{game_area.size.x / 2 - 1, 0, 0},
										   KEY_W,
										   KEY_S,
										   paddle_right_pipeline_instance,
										   1);
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

		fix16 height = fix16(camera.getZoomRatio());
		fix16 width = height * fix16(camera.aspectRatio());

		game_area = Area{{-width / 2, -height / 2},
						 {width,      height}};

		paddle_left_entity.get<PaddleComponent>().position.x = -game_area.size.x / 2 + 1;
		paddle_right_entity.get<PaddleComponent>().position.x = game_area.size.x / 2 - 1;
	}
}