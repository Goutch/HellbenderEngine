#pragma once

#include "HBE.h"
#include "Games/Pong/PongGameScene.h"

using namespace HBE;
namespace Pong {

	struct PaddleComponent {
		float speed;
		float size_x;
		float size_y;
		KEY up_key;
		KEY down_key;
	};

	class PaddleSystem : public System {
		PongGameScene *game_scene;
	public:
		PaddleSystem(PongGameScene *game_scene);

		~PaddleSystem() override;

		void update(float delta);

	};
}

