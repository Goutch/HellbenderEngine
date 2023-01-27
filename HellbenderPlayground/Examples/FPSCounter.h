#pragma once

#include "HBE.h"

namespace HBE {
	class FPSCounter {
		struct BackgroundMaterial {
			vec4 color;
			bool has_texture;
		};
		BackgroundMaterial material;
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

		Mesh *text_mesh;
		Font *font;

		std::string text;

		float total_width;
		float total_height;

		RenderTarget *render_target;

		Texture *background_texture;
	public:
		void update(float delta) {
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
			text_pipeline_instance->setUniform("material", &text_color);

			Geometry::createText(*text_mesh,
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
			background_entity.get<Transform>().setPosition(vec3(
					-static_cast<float>(render_target->getResolution().x) / 2.0f,
					-static_cast<float>(render_target->getResolution().y) / 2.0f,
					0.0f));
		}

		FPSCounter(RenderTarget *render_target) {
			Application::getScene()->onUpdate.subscribe(this, &FPSCounter::update);
			render_target->onResolutionChange.subscribe(this, &FPSCounter::onResolutionChange);
			this->render_target = render_target;
			createResources();
			setupScene();
		}

		~FPSCounter() {
			Application::getScene()->onUpdate.unsubscribe(this);
			delete text_vertex_shader;
			delete text_fragment_shader;
			delete background_vertex_shader;
			delete background_fragment_shader;
			delete text_pipeline;
			delete background_pipeline;
			delete text_pipeline_instance;
			delete background_pipeline_instance;
			delete quad;
			delete text_mesh;
			delete font;
		}


		void setupScene() {
			Scene *scene = Application::getScene();
			text_entity = scene->createEntity3D();
			MeshRenderer &text_renderer = text_entity.attach<MeshRenderer>();

			text_renderer.pipeline_instance = text_pipeline_instance;
			text_renderer.mesh = text_mesh;

			text_entity.get<Transform>().translate(vec3(1.0f, 1.0f, 0.0f));

			Geometry::createText(*text_mesh,
								 text,
								 *font,
								 1.0,
								 1.0,
								 TEXT_ALIGNMENT_LEFT,
								 PIVOT_CENTER,
								 total_width,
								 total_height);

			background_entity = scene->createEntity3D();
			MeshRenderer &background_renderer = background_entity.attach<MeshRenderer>();
			background_renderer.mesh = quad;
			background_renderer.pipeline_instance = background_pipeline_instance;

			scene->setParent(text_entity, background_entity);
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
			background_fragment_shader = Resources::createShader(fragment_shader_info);
			background_vertex_shader = Resources::createShader(vertex_shader_info);

			GraphicPipelineInfo pipeline_info{};
			pipeline_info.attribute_info_count = 1;

			pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D_UV_INTERLEAVED;
			pipeline_info.vertex_shader = text_vertex_shader;
			pipeline_info.fragement_shader = text_fragment_shader;
			pipeline_info.flags = 0;
			text_pipeline = Resources::createGraphicPipeline(pipeline_info);

			pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
			pipeline_info.vertex_shader = background_vertex_shader;
			pipeline_info.fragement_shader = background_fragment_shader;
			background_pipeline = Resources::createGraphicPipeline(pipeline_info);


			GraphicPipelineInstanceInfo pipeline_instance_info{};
			pipeline_instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;

			pipeline_instance_info.graphic_pipeline = text_pipeline;
			text_pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info);

			pipeline_instance_info.graphic_pipeline = background_pipeline;
			background_pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info);

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

			background_texture = Texture::load("textures/orange_lines_512.png", IMAGE_FORMAT_RGBA32F, IMAGE_FLAG_NONE);

			material = {
					.color = vec4(1.0f, 1.0f, 1.0f, 1.0f),
					.has_texture=true,
			};
			background_pipeline_instance->setUniform("material", &material);
			background_pipeline_instance->setUniform("texture0", background_texture);

			text_pipeline_instance->setTexture("mtsdf", font->getTextureAtlas());
		}
	};
}