#pragma once

#include "HBE.h"

using namespace HBE;
namespace Pong {
	struct Area {
		vec2 position;
		vec2 size;
	};

	class PongGameScene : public Scene {
		Area game_area;

		Mesh *quad_mesh;
		Shader *vertex_shader;
		Shader *fragment_shader;
		GraphicPipeline *pipeline;
		GraphicPipelineInstance *pipeline_instance;
		RenderTarget *render_target;
	public:
		PongGameScene();

		~PongGameScene() override;

		void OnWindowSizeChange(Window *window);

		void setupScene();

		void createResources();

		Area &getArea();

		void onRenderTargetResolutionChange(RenderTarget* render_target);
	};
}
