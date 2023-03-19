#pragma once

#include "HBE.h"
#include "PongGameScene.h"
#include "Simulation/fixed.h"
#include "Simulation/Commands.h"

using namespace locknet;
using namespace HBE;
namespace PongLockstep {

	struct PaddleComponent {
		fix16 speed;
		fix16 size_x;
		fix16 size_y;
		vec2fix16 position;
		KEY up_key;
		KEY down_key;
		int client_id;
	};

	class PaddleSystem : public System {
		PongGameScene *game_scene;
		PongGameState *game_state;
	public:
		PaddleSystem(PongGameScene *game_scene,PongGameState &game_state);

		~PaddleSystem() override;

		void step(StepData step,StepInputsData* step_input);
		void update(float delta);

	};
}

