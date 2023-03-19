#pragma once

#include "HBE.h"
#include "PongGameScene.h"
#include "PongUIScene.h"
#include "Simulation/Commands.h"
#include "Simulation/Client.h"

using namespace fpm;
using namespace HBE;

namespace PongLockstep {
	struct PongGameState {
		uint32 score_left;
		uint32 score_right;
		Event<StepData,StepInputsData*> onStep;
		Client *client=nullptr;
	};

	class PongGame {
	public:
		const static vec4 LEFT_COLOR;
		const static vec4 RIGHT_COLOR;
	private:
		PongUIScene *ui_scene;
		PongGameScene *game_scene;
		PongGameState game_state;

	public :
		PongGame(const std::string& ip);

		~PongGame();

	private:
		void onPresent();
		void onUpdate(float delta);
		void onEventReceived(void* data);
		void reset(PongGameState &game_state,const std::string& ip);
	};
}

