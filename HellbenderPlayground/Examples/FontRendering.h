#pragma once

#include "HBE.h"

using namespace HBE;

class FontRenderingScene {

public:
	FontRenderingScene() {
		std::vector<VertexAttributeInfo> attributes;
		attributes.emplace_back();
		attributes[0].location = 0;
		attributes[0].size = sizeof(vec3) + sizeof(vec2);
		attributes[0].flags = VERTEX_ATTRIBUTE_FLAG_NONE;

		Shader *text_vertex_shader;
		Shader *text_fragment_shader;
		ShaderInfo vertex_shader_info{};
		ShaderInfo fragment_shader_info{};
		vertex_shader_info.stage = SHADER_STAGE_VERTEX;
		fragment_shader_info.stage = SHADER_STAGE_FRAGMENT;
		vertex_shader_info.path = "shaders/defaults/textMSDF.vert";
		fragment_shader_info.path = "shaders/defaults/textMSDF.frag";
		text_vertex_shader = Resources::createShader(vertex_shader_info, "vertex");
		text_fragment_shader = Resources::createShader(fragment_shader_info, "fragment");

		GraphicPipeline *text_pipeline;
		GraphicPipelineInfo pipeline_info{};
		pipeline_info.attribute_infos = attributes.data();
		pipeline_info.attribute_info_count = attributes.size();
		pipeline_info.vertex_shader = text_vertex_shader;
		pipeline_info.fragement_shader = text_fragment_shader;
		pipeline_info.topology = VERTEX_TOPOLOGY_TRIANGLE;
		pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_NONE;
		text_pipeline = Resources::createGraphicPipeline(pipeline_info, "pipeline");

		GraphicPipelineInstance *text_pipeline_instance;
		GraphicPipelineInstanceInfo pipeline_instance_info{};
		pipeline_instance_info.graphic_pipeline = text_pipeline;
		pipeline_instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;
		text_pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info, "pipeline_instance");


		Scene *scene = Application::getScene();

		Entity camera_entity = scene->createEntity();

		Camera2D camera2D = camera_entity.attach<Camera2D>();

		scene->setCameraEntity(camera_entity);

		std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{};':\",./<>?\\|`~";
		FontInfo font_info{};
		font_info.path = "fonts/OpenSans-Regular.ttf";
		font_info.characters = characters.data();
		font_info.characters_count = characters.size();
		font_info.glyph_size = 64;
		Font *font = Resources::createFont(font_info, "font");
		text_pipeline_instance->setTexture("mtsdf", font->getTextureAtlas());


		Entity text_entity = scene->createEntity();
		TextRenderer &text_renderer = text_entity.attach<TextRenderer>();

		std::string s = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";//Last A make string crash here
		text_renderer.pipeline_instance = text_pipeline_instance;
		text_renderer.text =s;
		text_renderer.font = font;
		text_renderer.space_width=0.5f;
		text_renderer.buildMesh();
	}

};