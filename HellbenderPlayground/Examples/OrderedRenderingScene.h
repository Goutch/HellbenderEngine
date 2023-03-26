#pragma once

#include "HBE.h"

using namespace HBE;

class OrderedRenderingScene : public Scene {
	Mesh *triangle_mesh;

	Shader *vertex_shader;
	Shader *fragment_shader;
	GraphicPipeline *pipeline;
	std::vector<GraphicPipelineInstance *> pipeline_instances;

	RenderTarget *render_target;

	FPSCounter *fps_counter;
public:

	OrderedRenderingScene() {
		createResources();
		setupScene();
		fps_counter = new FPSCounter(*this, render_target);
	}

	~OrderedRenderingScene() {
		delete fps_counter;
		delete triangle_mesh;
		delete vertex_shader;
		delete fragment_shader;
		delete pipeline;
		for (auto pipeline_instance: pipeline_instances) {
			delete pipeline_instance;
		}
		delete render_target;

	}

	Entity createTriangle(vec3 position, vec4 color) {

		GraphicPipelineInstanceInfo pipeline_instance_info{};
		pipeline_instance_info.graphic_pipeline = pipeline;
		pipeline_instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;
		GraphicPipelineInstance *pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info);

		pipeline_instances.push_back(pipeline_instance);
		pipeline_instance->setUniform("material", &color);

		Entity triangle_entity = createEntity3D();
		MeshRenderer &triangle_renderer = triangle_entity.attach<MeshRenderer>();
		triangle_renderer.mesh = triangle_mesh;
		triangle_renderer.pipeline_instance = pipeline_instance;
		triangle_renderer.ordered = true;
		triangle_renderer.layer = 1;
		triangle_entity.get<Transform>().setPosition(position);

		return triangle_entity;

	}


	void setupScene() {
		Entity camera_entity = createEntity3D();
		PixelCamera &camera = camera_entity.attach<PixelCamera>();
		camera.setRenderTarget(render_target);
		camera.layer_mask = 1;

		std::vector<Entity> entitities;
		int n = 0;
		for (int i = 0; i < n; ++i) {
			Entity triangle = createTriangle(vec3(Random::floatRange(0, render_target->getResolution().x), Random::floatRange(0, render_target->getResolution().y),0),
											 vec4(Random::floatRange(0, 1),
												  Random::floatRange(0, 1),
												  Random::floatRange(0, 1),
												  1));

			entitities.push_back(triangle);
			triangle.get<Transform>().setWorldScale(vec3(Random::floatRange(10, 300), Random::floatRange(10, 300), 1));
		}

		printSceneHierarchy();
	}

	void onWindowSizeChange(Window *window) {
		render_target->setResolution(window->getWidth(), window->getHeight());
	}

	void createResources() {
		RenderTargetInfo render_target_info{};
		render_target_info.clear_color = vec4(0, 0, 0, 0);
		render_target_info.width = Graphics::getWindow()->getWidth();
		render_target_info.height = Graphics::getWindow()->getHeight();
		render_target_info.flags = RENDER_TARGET_FLAG_CLEAR_COLOR | RENDER_TARGET_FLAG_COLOR_ATTACHMENT;
		render_target = Resources::createRenderTarget(render_target_info);


		MeshInfo triangle_info{};
		triangle_info.attribute_info_count = 1;
		triangle_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;

		triangle_mesh = Resources::createMesh(triangle_info);

		float vertices[] = {
				-0.5f, -0.5f, 0.0f,
				0.5f, -0.5f, 0.0f,
				0.0f, 0.5f, 0.0f
		};
		triangle_mesh->setBuffer(0, vertices, 3);


		ShaderInfo vertex_shader_info{};
		ShaderInfo fragment_shader_info{};

		vertex_shader_info.path = "shaders/defaults/Position.vert";
		vertex_shader_info.stage = SHADER_STAGE_VERTEX;

		fragment_shader_info.path = "shaders/defaults/Position.frag";
		fragment_shader_info.stage = SHADER_STAGE_FRAGMENT;

		vertex_shader = Resources::createShader(vertex_shader_info);
		fragment_shader = Resources::createShader(fragment_shader_info);

		GraphicPipelineInfo pipeline_info{};
		pipeline_info.attribute_info_count = 1;
		pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
		pipeline_info.vertex_shader = vertex_shader;
		pipeline_info.fragment_shader = fragment_shader;
		pipeline_info.render_target = render_target;
		pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_IGNORE_DEPTH_TEST;

		pipeline = Resources::createGraphicPipeline(pipeline_info);

		GraphicPipelineInstanceInfo pipeline_instance_info{};
		pipeline_instance_info.graphic_pipeline = pipeline;
		pipeline_instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;

		Graphics::getWindow()->onSizeChange.subscribe(this, &OrderedRenderingScene::onWindowSizeChange);
	}

};