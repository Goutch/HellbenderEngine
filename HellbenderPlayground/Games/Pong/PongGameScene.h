#pragma once

#include "HBE.h"

using namespace HBE;
namespace Pong {
	struct Area {
		vec2 position;
		vec2 size;
	};

	struct PongGameState;

	class PongGameScene : public Scene {
		Area game_area;

		Mesh *quad_mesh;
		Shader *vertex_shader;
		Shader *fragment_shader;
		GraphicPipeline *pipeline;
		GraphicPipelineInstance *ball_pipeline_instance;
		GraphicPipelineInstance *paddle_left_pipeline_instance;
		GraphicPipelineInstance *paddle_right_pipeline_instance;
		Entity paddle_left_entity;
		Entity paddle_right_entity;
		RenderTarget *render_target;
	public:
		PongGameScene(PongGameState &game_state);

		~PongGameScene() override;

		void OnWindowSizeChange(Window *window);

		void setupScene();

		void createResources();

		Area &getArea();

		void onRenderTargetResolutionChange(RenderTarget *render_target);

		Entity createPaddle(vec3 position, KEY up_key, KEY down_key, GraphicPipelineInstance *paddle_pipeline_instance);


	public:
		Entity createBall(vec2 position,vec2 velocity);
	};
}
