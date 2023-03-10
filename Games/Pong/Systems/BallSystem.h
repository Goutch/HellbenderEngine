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
		float t_since_last_bounce=0;
		PongGameScene *game_scene;
		PongGameState *game_state;
		AudioClipInstance *bounce_sound;
	public:
		BallSystem(PongGameScene *scene, PongGameState &game_state,AudioClipInstance *bounce_sound);

		void update(float delta);
	};
}

