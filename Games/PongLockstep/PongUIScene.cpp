
#include "PongUIScene.h"
#include "PongGame.h"

namespace PongLockstep {


	void PongUIScene::OnWindowSizeChange(Window *window) {
		render_target->setResolution(window->getWidth(), window->getHeight());
		score_left_entity.get<Transform>().setPosition(vec3((window->getWidth() / 4.0), window->getHeight() - 20, 0));
		score_right_entity.get<Transform>().setPosition(vec3(((window->getWidth() * 3) / 4.0), window->getHeight() - 20, 0));

		score_right_entity.get<Transform>().setLocalScale(vec3(SCORE_TEXT_SIZE, SCORE_TEXT_SIZE, 1));
		score_right_entity.get<Transform>().setLocalScale(vec3(SCORE_TEXT_SIZE, SCORE_TEXT_SIZE, 1));
	}

	Entity PongUIScene::createScore(GraphicPipelineInstance *pipeline_instance, Mesh *text) {
		Entity score = createEntity3D();
		MeshRenderer &score_renderer = score.attach<MeshRenderer>();
		score_renderer.pipeline_instance = pipeline_instance;
		score_renderer.mesh = text;
		score_renderer.ordered = true;
		score.get<Transform>().setLocalScale(vec3(SCORE_TEXT_SIZE, SCORE_TEXT_SIZE, 1));

		return score;
	}

	PongUIScene::PongUIScene(PongGameState &game_state) {
		this->game_state = &game_state;

		createResources();

		setupScene();
		Entity camera_entity = createEntity3D();
		PixelCamera &camera = camera_entity.attach<PixelCamera>();
		camera.setRenderTarget(render_target);

		this->onUpdate.subscribe(this, &PongUIScene::updateUI);

		Graphics::getWindow()->onSizeChange.subscribe(this, &PongUIScene::OnWindowSizeChange);
	}

	PongUIScene::~PongUIScene() {
		this->onUpdate.unsubscribe(this);
		Graphics::getWindow()->onSizeChange.unsubscribe(this);

		delete score_left_mesh;
		delete score_right_mesh;
		delete left_text_pipeline_instance;
		delete right_text_pipeline_instance;
		delete fps_counter;
		delete render_target;
	}


	void PongUIScene::createResources() {
		RenderTargetInfo render_target_info{};
		render_target_info.clear_color = vec4(0, 0, 0, 0);
		render_target_info.width = Graphics::getWindow()->getWidth();
		render_target_info.height = Graphics::getWindow()->getHeight();
		render_target_info.flags = RENDER_TARGET_FLAG_CLEAR_COLOR | RENDER_TARGET_FLAG_COLOR_ATTACHMENT;
		render_target = Resources::createRenderTarget(render_target_info);


		fps_counter = new FPSCounter(*this, render_target);
		pipeline = fps_counter->getPipeline();
		font = fps_counter->getFont();


		float text_height, text_width;
		score_left_mesh = Geometry::createText(std::to_string(game_state->score_left), *font, 1, 1, TEXT_ALIGNMENT_LEFT, PIVOT_TOP_LEFT, text_width, text_height);
		score_right_mesh = Geometry::createText(std::to_string(game_state->score_right), *font, 1, 1, TEXT_ALIGNMENT_RIGHT, PIVOT_TOP_RIGHT, text_width, text_height);


		GraphicPipelineInstanceInfo pipeline_instance_info{};
		pipeline_instance_info.graphic_pipeline = pipeline;
		pipeline_instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;

		left_text_pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info);
		right_text_pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info);

		left_text_pipeline_instance->setUniform("material", &PongGame::LEFT_COLOR);
		right_text_pipeline_instance->setUniform("material", &PongGame::RIGHT_COLOR);
		left_text_pipeline_instance->setTexture("mtsdf", font->getTextureAtlas());
		right_text_pipeline_instance->setTexture("mtsdf", font->getTextureAtlas());
	}

	void PongUIScene::setupScene() {
		score_left_entity = createScore(left_text_pipeline_instance, score_left_mesh);
		score_right_entity = createScore(right_text_pipeline_instance, score_right_mesh);

		last_score_left = game_state->score_left;
		last_score_right = game_state->score_right;

		score_left_entity.get<Transform>().setPosition(vec3(((render_target->getResolution().x) / 4.0), render_target->getResolution().y - 20, 0));

		score_right_entity.get<Transform>().setPosition(vec3(((render_target->getResolution().x * 3) / 4.0), render_target->getResolution().y - 20, 0));
	}

	void PongUIScene::updateUI(float delta) {
		if (last_score_left != game_state->score_left) {
			last_score_left = game_state->score_left;
			float text_height, text_width;
			Geometry::updateText(*score_left_mesh, std::to_string(game_state->score_left), *font, 1, 1, TEXT_ALIGNMENT_LEFT, PIVOT_TOP_CENTER, text_width, text_height);
		}
		if (last_score_right != game_state->score_right) {
			last_score_right = game_state->score_right;
			float text_height, text_width;
			Geometry::updateText(*score_right_mesh, std::to_string(game_state->score_right), *font, 1, 1, TEXT_ALIGNMENT_RIGHT, PIVOT_TOP_CENTER, text_width, text_height);
		}

	}
}