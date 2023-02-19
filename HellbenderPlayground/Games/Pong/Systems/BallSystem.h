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
		float t=0;
		PongGameScene *game_scene;
		PongGameState *game_state;
	public:
		BallSystem(PongGameScene *scene, PongGameState &game_state);

		void update(float delta);
	};
}

