#pragma once

#include "HBE.h"

using namespace HBE;

class TextScene : public Scene {
	std::string text_str = "Hello world!\nThis is a test of the text rendering system\nabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{};':\",./<>?\\|`~";

	Shader *text_vertex_shader;
	Shader *text_fragment_shader;
	Mesh *text_mesh;
	GraphicPipeline *text_pipeline;
	GraphicPipelineInstance *text_pipeline_instance;
	Font *font;

	void onCharacterInput(char codepoint) {
		if (getCameraEntity().has<CameraController>())
			return;
		if (codepoint == 'c')
			return;
		text_str += codepoint;
		float total_width;
		float total_height;
		Geometry::updateText(*text_mesh,
							 text_str,
							 *font,
							 1.0,
							 1.0,
							 TEXT_ALIGNMENT_LEFT,
							 PIVOT_CENTER,
							 total_width,
							 total_height);

	}

	void onUpdate(float delta) {
		if (getCameraEntity().has<CameraController>())
			return;
		if (Input::getKeyDown(KEY_ENTER)) {
			onCharacterInput(static_cast<char>('\n'));
		}
		if (Input::getKeyDown(KEY_BACKSPACE)) {
			text_str.pop_back();
			float total_width;
			float total_height;
			Geometry::updateText(*text_mesh,
								 text_str,
								 *font,
								 1.0,
								 1.0,
								 TEXT_ALIGNMENT_LEFT,
								 PIVOT_CENTER,
								 total_width,
								 total_height);
		}
	}

private:
	void createResources() {
		ShaderInfo vertex_shader_info{};
		ShaderInfo fragment_shader_info{};
		vertex_shader_info.stage = SHADER_STAGE_VERTEX;
		fragment_shader_info.stage = SHADER_STAGE_FRAGMENT;

		vertex_shader_info.path = "shaders/defaults/TextMSDF.vert";
		fragment_shader_info.path = "shaders/defaults/TextMSDF.frag";
		text_vertex_shader = Resources::createShader(vertex_shader_info);
		text_fragment_shader = Resources::createShader(fragment_shader_info);

		GraphicPipelineInfo pipeline_info{};
		pipeline_info.attribute_info_count = 1;

		pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED;
		pipeline_info.vertex_shader = text_vertex_shader;
		pipeline_info.fragment_shader = text_fragment_shader;
		pipeline_info.flags = 0;
		text_pipeline = Resources::createGraphicPipeline(pipeline_info);

		GraphicPipelineInstanceInfo pipeline_instance_info{};
		pipeline_instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;

		pipeline_instance_info.graphic_pipeline = text_pipeline;
		text_pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info);

	}

	void setupScene() {
		Entity camera_entity = createEntity3D();

		Camera2D camera2D = camera_entity.attach<Camera2D>();

		std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{};':\",./<>?\\|`~";
		FontInfo font_info{};
		font_info.path = "fonts/OpenSans-Regular.ttf";
		font_info.characters = characters.data();
		font_info.characters_count = characters.size();
		font_info.glyph_resolution = 64;

		font = Resources::createFont(font_info);
		text_pipeline_instance->setTexture("mtsdf", font->getTextureAtlas());

		Entity text_entity = createEntity3D();
		MeshRenderer &text_renderer = text_entity.attach<MeshRenderer>();

		text_renderer.pipeline_instance = text_pipeline_instance;
		text_renderer.mesh = text_mesh;
	}

public:
	TextScene() {
		createResources();
		setupScene();
		Input::onCharDown.subscribe(this, &TextScene::onCharacterInput);
		Application::onUpdate.subscribe(this, &TextScene::onUpdate);
	}

	~TextScene() {
		delete text_vertex_shader;
		delete text_fragment_shader;
		delete text_pipeline_instance;
		delete text_pipeline;
		delete font;
		delete text_mesh;
	}

};