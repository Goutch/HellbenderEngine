#pragma once

#include "HBE.h"

namespace HBE {
	class FPSCounter {
		Shader *text_vertex_shader;
		Shader *text_fragment_shader;
		Shader *background_vertex_shader;
		Shader *background_fragment_shader;

		GraphicPipeline *text_pipeline;
		GraphicPipeline *background_pipeline;

		GraphicPipelineInstance *text_pipeline_instance;
		GraphicPipelineInstance *background_pipeline_instance;


		Entity text_entity;
		Entity background_entity;

		Mesh *quad;

		Font *font;

		std::string text;
	public:
		void update(float delta) {
			TextRenderer &text_renderer = text_entity.get<TextRenderer>();


			int fps = (int) std::floor(1 / delta);

			text = "FPS: " + std::to_string(fps);

			vec4 text_color = vec4(1, 1, 1, 1);

			if (fps < 60)
				text_color = vec4(1, 0, 0, 1);
			else if (fps < 120) {
				text_color = vec4(1, 1, 0, 1);
			} else {
				text_color = vec4(0, 1, 0, 1);
			}
			text_renderer.pipeline_instance->setUniform("material", &text_color);
			text_renderer.text = text.c_str();
			text_renderer.text_length = text.length();

			text_renderer.buildMesh();
		}

		~FPSCounter() {
			Application::getScene()->onUpdate.unsubscribe(this);
		}

		void init() {
			//Scene setup
			Scene *scene = Application::getScene();
			text_entity = scene->createEntity3D();
			TextRenderer &text_renderer = text_entity.attach<TextRenderer>();

			text_renderer.pipeline_instance = text_pipeline_instance;
			text_renderer.font = font;
			text_renderer.text = text.c_str();
			text_renderer.text_length = 0;
			text_renderer.space_width = 0.4f;
			text_renderer.line_height = 1.0f;


			text_renderer.alignment = TEXT_ALIGNMENT_LEFT;
			text_renderer.pivot = PIVOT_TOP_LEFT;
			text_entity.get<Transform>().translate(vec3(1.0f, 1.0f, 0.0f));

			text_renderer.buildMesh();

			background_entity = scene->createEntity3D();
			UIRenderer &background_renderer = background_entity.attach<UIRenderer>();
			background_renderer.mesh = quad;
			background_renderer.pipeline_instance = background_pipeline_instance;

			struct Material {
				vec4 color;
				bool has_texture;
			};

			Material mat{
					vec4(1.0f, 0.0f, 0.0f, 1.0f),
					false
			};
			background_pipeline_instance->setUniform("material", &mat);
		}

		FPSCounter() {
			//resrouces creation
			text = new char[32];
			Application::getScene()->onUpdate.subscribe(this, &FPSCounter::update);

			ShaderInfo vertex_shader_info{};
			ShaderInfo fragment_shader_info{};
			vertex_shader_info.stage = SHADER_STAGE_VERTEX;
			fragment_shader_info.stage = SHADER_STAGE_FRAGMENT;

			vertex_shader_info.path = "shaders/defaults/TextMSDF.vert";
			fragment_shader_info.path = "shaders/defaults/TextMSDF.frag";
			text_vertex_shader = Resources::createShader(vertex_shader_info, "text_vertex");
			text_fragment_shader = Resources::createShader(fragment_shader_info, "text_fragment");

			vertex_shader_info.path = "shaders/defaults/PositionUV.vert";
			fragment_shader_info.path = "shaders/defaults/PositionUV.frag";
			background_fragment_shader = Resources::createShader(fragment_shader_info, "background_vertex");
			background_vertex_shader = Resources::createShader(vertex_shader_info, "background_fragment");

			GraphicPipelineInfo pipeline_info{};
			pipeline_info.attribute_info_count = 1;

			pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED;
			pipeline_info.vertex_shader = text_vertex_shader;
			pipeline_info.fragement_shader = text_fragment_shader;
			pipeline_info.flags = 0;
			text_pipeline = Resources::createGraphicPipeline(pipeline_info, "text_pipeline");

			pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
			pipeline_info.vertex_shader = background_vertex_shader;
			pipeline_info.fragement_shader = background_fragment_shader;
			background_pipeline = Resources::createGraphicPipeline(pipeline_info, "background_pipeline");


			GraphicPipelineInstanceInfo pipeline_instance_info{};
			pipeline_instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;

			pipeline_instance_info.graphic_pipeline = text_pipeline;
			text_pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info, "text_pipeline_instance");

			pipeline_instance_info.graphic_pipeline = background_pipeline;
			background_pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info, "background_pipeline_instance");


			MeshInfo mesh_info{};
			mesh_info.attribute_info_count = 1;
			mesh_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED;
			quad = Resources::createMesh(mesh_info, "FPSCounterBackgroundMesh");
			Geometry::createQuad(*quad, 1, 1, VERTEX_FLAG_UV, PIVOT_TOP_LEFT);

			std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{};':\",./<>?\\|`~";
			FontInfo font_info{};
			font_info.path = "fonts/OpenSans-Regular.ttf";
			font_info.characters = characters.data();
			font_info.characters_count = characters.size();
			font_info.glyph_resolution = 64;

			font = Resources::createFont(font_info, "font");
			text_pipeline_instance->setTexture("mtsdf", font->getTextureAtlas());
		}
	};
}