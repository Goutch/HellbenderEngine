#pragma once

#include "HBE.h"
#include "PongGameScene.h"
#include "PongUIScene.h"

using namespace HBE;

namespace Pong {
	struct PongGameState {
		uint32 score_left;
		uint32 score_right;
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
		PongGame();

		~PongGame();

	private:
		void onPresent();
	};
}

