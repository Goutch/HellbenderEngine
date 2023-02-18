
#include "PongGameScene.h"
#include "Games/Pong/Systems/BallSystem.h"

namespace Pong {

	Area &PongGameScene::getArea() {
		return game_area;
	}

	PongGameScene::PongGameScene() {
		createResources();
		setupScene();
		Graphics::getWindow()->onSizeChange.subscribe(this, &PongGameScene::OnWindowSizeChange);

		addSystem(new BallSystem(this));
	}

	PongGameScene::~PongGameScene() {
		render_target->onResolutionChange.unsubscribe(this);
		delete quad_mesh;
		delete vertex_shader;
		delete fragment_shader;
		delete pipeline;
		delete pipeline_instance;
		delete render_target;
	}

	void PongGameScene::setupScene() {
		Entity camera_entity = createEntity3D();
		Camera2D &camera = camera_entity.attach<Camera2D>();
		camera.setRenderTarget(render_target);

		Entity ball = createEntity3D();
		MeshRenderer &ball_renderer = ball.attach<MeshRenderer>();
		ball_renderer.mesh = quad_mesh;
		ball_renderer.layer = 0;
		ball_renderer.pipeline_instance = pipeline_instance;

		BallComponent &ballComponent = ball.attach<BallComponent>();
		ballComponent.velocity = vec2{10, 10};
		ballComponent.radius = 0.5;

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

		pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info);

		vec4 color = {1, 0, 0, 1};
		pipeline_instance->setUniform("material", &color);
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
	}
}