#pragma once

#include "HBE.h"
#include "simulation/fixed.h"

using namespace HBE;
using namespace locknet;
namespace PongLockstep {
	struct Area {
		vec2fix16 position;
		vec2fix16 size;
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
		AudioClip *bounce_sound;
		AudioClipInstance *bounce_sound_instance;
	public:
		PongGameScene(PongGameState &game_state);

		~PongGameScene() override;

		void OnWindowSizeChange(Window *window);

		void setupScene();

		void createResources();

		Area &getArea();

		void onRenderTargetResolutionChange(RenderTarget *render_target);

		Entity createPaddle(vec3 position, KEY up_key, KEY down_key, GraphicPipelineInstance *paddle_pipeline_instance, uint32_t client_id);


	public:
		Entity createBall(vec2fix16 position, vec2fix16 velocity);
	};
}
