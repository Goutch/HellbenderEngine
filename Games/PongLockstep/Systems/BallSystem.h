#pragma once

#include "HBE.h"
#include "PongGameScene.h"
#include "Simulation/Commands.h"

using namespace locknet;
using namespace HBE;
namespace PongLockstep {

	struct BallComponent {
		vec2fix16 velocity;
		vec2fix16 position;
		fix16 radius;
	};

	struct PongGameState;

	class BallSystem : public System {
		fix16 t = fix16(0);
		float t_since_last_bounce = 0;
		PongGameScene *game_scene;
		PongGameState *game_state;
		AudioClipInstance *bounce_sound;
	public:
		BallSystem(PongGameScene *scene, PongGameState &game_state, AudioClipInstance *bounce_sound);

		~BallSystem() override;

		void step(StepData step, StepInputsData *inputs);

		void update(float delta);
	};
}

