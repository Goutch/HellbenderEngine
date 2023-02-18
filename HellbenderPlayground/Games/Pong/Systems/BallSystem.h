#pragma once

#include "HBE.h"
#include "Games/Pong/PongGameScene.h"

using namespace HBE;
namespace Pong {

	struct BallComponent {
		vec2 velocity;
		float radius;
	};

	class BallSystem : public System {
		PongGameScene *game_scene;
	public:
		BallSystem(PongGameScene *scene);

		void update(float delta);
	};
}

