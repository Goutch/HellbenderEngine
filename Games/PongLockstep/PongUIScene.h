#pragma once

#include "HBE.h"
#include "Games/PongLockstep/FPSCounter.h"

using namespace HBE;
namespace PongLockstep {
	struct PongGameState;

	class PongUIScene : public Scene {
		const float SCORE_TEXT_SIZE = 50.0f;
		FPSCounter *fps_counter;
		RenderTarget *render_target;
		PongGameState *game_state;

		Font *font;
		GraphicPipeline *pipeline;

		GraphicPipelineInstance *left_text_pipeline_instance;
		GraphicPipelineInstance *right_text_pipeline_instance;

		Mesh *score_left_mesh;
		Mesh *score_right_mesh;

		Entity score_left_entity;
		Entity score_right_entity;

		uint32_t last_score_left = 0;
		uint32_t last_score_right = 0;
	public:
		void updateUI(float delta);
		PongUIScene(PongGameState &game_state);

		~PongUIScene() override;

		void OnWindowSizeChange(Window *window);

		Entity createScore(GraphicPipelineInstance *pipeline_instance, Mesh *text);

		void createResources();
		void setupScene();
	};
}

