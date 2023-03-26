#pragma once

#include "HBE.h"

#define N_DELTA 100
namespace HBE {
	class FPSCounter {
		Shader *text_vertex_shader;
		Shader *text_fragment_shader;

		GraphicPipeline *text_pipeline;

		GraphicPipelineInstance *text_pipeline_instance;
		Entity text_entity;

		Mesh *quad;

		Mesh *text_mesh;
		Font *font;

		std::string text;

		float total_width;
		float total_height;

		RenderTarget *render_target;
		Scene *scene;

		vec4 red = vec4(251.0f / 255.0f, 73.0f / 255.0f, 52.0f / 255.0f, 1);
		vec4 yellow = vec4(250.0f / 255.0f, 189.0f / 255.0f, 47.0f / 255.0f, 1);
		vec4 green = vec4(184.0f / 255.0f, 250.0f / 255.0f, 38.0f / 255.0f, 1);

		float time_since_last_fps_update = 0.0f;
	public:
		void update(float delta) {
			time_since_last_fps_update += delta;
			if (time_since_last_fps_update < 1.0f) {
				return;
			}
			time_since_last_fps_update = 0.0f;
			int fps = (int) std::floor(1.0f / delta);

			text = std::to_string(fps) + "fps\n" +
				   std::to_string(delta * 1000.0f) + "ms";


			vec4 text_color = vec4(1, 1, 1, 1);

			if (fps < 60)
				text_color = red;
			else if (fps < 120) {
				text_color = yellow;
			} else {
				text_color = green;
			}
			text_pipeline_instance->setUniform("material", &text_color);

			Geometry::updateText(*text_mesh,
								 text,
								 *font,
								 1.0,
								 1.0,
								 TEXT_ALIGNMENT_LEFT,
								 PIVOT_TOP_LEFT,
								 total_width,
								 total_height);


		}

		void onResolutionChange(RenderTarget *render_target) {
			text_entity.get<Transform>().setLocalScale(vec3(20));

			text_entity.get<Transform>().setPosition(vec3(
					0,
					render_target->getResolution().y,
					0.0f));
		}


		FPSCounter(Scene &scene, RenderTarget *render_target) {
			this->scene = &scene;
			scene.onUpdate.subscribe(this, &FPSCounter::update);
			render_target->onResolutionChange.subscribe(this, &FPSCounter::onResolutionChange);
			this->render_target = render_target;
			createResources();
			setupScene();
			onResolutionChange(render_target);
		}

		~FPSCounter() {
			text_entity.destroy();
			scene->onUpdate.unsubscribe(this);
			render_target->onResolutionChange.unsubscribe(this);
			delete text_vertex_shader;
			delete text_fragment_shader;
			delete text_pipeline;
			delete text_pipeline_instance;

			delete quad;
			delete text_mesh;
			delete font;
		}


		void setupScene() {

			text_entity = scene->createEntity3D();
			MeshRenderer &text_renderer = text_entity.attach<MeshRenderer>();

			text_renderer.pipeline_instance = text_pipeline_instance;
			text_renderer.mesh = text_mesh;
			text_renderer.ordered = true;

			text_entity.get<Transform>().translate(vec3(1.0f, 1.0f, 0.0f));
		}

		void createResources() {
			ShaderInfo vertex_shader_info{};
			ShaderInfo fragment_shader_info{};
			vertex_shader_info.stage = SHADER_STAGE_VERTEX;
			fragment_shader_info.stage = SHADER_STAGE_FRAGMENT;

			vertex_shader_info.path = "shaders/defaults/TextMSDF.vert";
			fragment_shader_info.path = "shaders/defaults/TextMSDF.frag";
			text_vertex_shader = Resources::createShader(vertex_shader_info);
			text_fragment_shader = Resources::createShader(fragment_shader_info);

			vertex_shader_info.path = "shaders/defaults/PositionUV.vert";
			fragment_shader_info.path = "shaders/defaults/PositionUV.frag";

			GraphicPipelineInfo pipeline_info{};
			pipeline_info.attribute_info_count = 1;

			pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED;
			pipeline_info.vertex_shader = text_vertex_shader;
			pipeline_info.fragment_shader = text_fragment_shader;
			pipeline_info.flags = 0;
			pipeline_info.render_target = render_target;
			text_pipeline = Resources::createGraphicPipeline(pipeline_info);

			pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
			pipeline_info.flags = 0;
			pipeline_info.render_target = render_target;


			GraphicPipelineInstanceInfo pipeline_instance_info{};
			pipeline_instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;

			pipeline_instance_info.graphic_pipeline = text_pipeline;
			text_pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info);

			MeshInfo mesh_info{};
			mesh_info.attribute_info_count = 1;
			mesh_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED;
			quad = Resources::createMesh(mesh_info);
			Geometry::createQuad(*quad, 1, 1, VERTEX_FLAG_UV, PIVOT_TOP_LEFT);

			std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{};':\",./<>?\\|`~";
			FontInfo font_info{};
			font_info.path = "fonts/OpenSans-Regular.ttf";
			font_info.characters = characters.data();
			font_info.characters_count = characters.size();
			font_info.glyph_resolution = 64;

			font = Resources::createFont(font_info);


			text_pipeline_instance->setTexture("mtsdf", font->getTextureAtlas());
			text = std::string("fps\n") + "ms";
			text_mesh = Geometry::createText(text,
											 *font,
											 1.0,
											 1.0,
											 TEXT_ALIGNMENT_LEFT,
											 PIVOT_CENTER,
											 total_width,
											 total_height);

		}

		GraphicPipeline *getPipeline() {
			return text_pipeline;
		}

		Font *getFont() {
			return font;
		}

		Shader *getVertexShader() {
			return text_vertex_shader;
		}

		Shader *getFragmentShader() {
			return text_fragment_shader;
		}

	};
}