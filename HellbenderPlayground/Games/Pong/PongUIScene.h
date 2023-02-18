#pragma once

#include "HBE.h"
#include "Examples/FPSCounter.h"

using namespace HBE;
namespace Pong {
	class PongUIScene : public Scene {
		FPSCounter *fps_counter;
		RenderTarget *render_target;
	public:
		PongUIScene();

		~PongUIScene() override;

		void OnWindowSizeChange(Window *window);
	};
}

