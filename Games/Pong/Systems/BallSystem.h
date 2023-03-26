#pragma once

#include "HBE.h"
#include "Games/Pong/PongGameScene.h"

using namespace HBE;
namespace Pong {

	struct BallComponent {
		vec2 velocity;
		float radius;
	};

	struct PongGameState;

	class BallSystem : public System {
		std::vector<mat4> ball_transforms;
		float t = 0;
		float t_since_last_bounce = 0;
		PongGameScene *game_scene;
		PongGameState *game_state;
		AudioClipInstance *bounce_sound;

		Mesh *ball_mesh;
		GraphicPipelineInstance *ball_pipeline_instance;
		GraphicPipeline *ball_pipeline;
		Shader *ball_vertex_shader;
		Shader *ball_fragment_shader;
	public:
		BallSystem(PongGameScene *scene, PongGameState &game_state, AudioClipInstance *bounce_sound, RenderTarget *render_target);

		~BallSystem();

		void onAttachBallComponent(Entity entity);

		void onDetachBallComponent(Entity entity);

		void draw(RenderGraph *render_graph);

		void update(float delta);
	};
}

