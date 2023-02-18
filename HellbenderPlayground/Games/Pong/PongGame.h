#pragma once

#include "HBE.h"
#include "PongGameScene.h"
#include "PongUIScene.h"

using namespace HBE;

namespace Pong {
	class PongGame {
		PongUIScene ui_scene;
		PongGameScene game_scene;
	public :
		PongGame();

		~PongGame();
	private:
		void onPresent();
	};
}

