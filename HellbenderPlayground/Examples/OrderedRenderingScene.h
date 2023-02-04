#pragma once

#include "HBE.h"

using namespace HBE;

class OrderedRenderingScene : public Scene {
	Mesh *triangle_mesh;

	Shader *vertex_shader;
	Shader *fragment_shader;
	GraphicPipeline *pipeline;
	std::vector<GraphicPipelineInstance *> pipeline_instances;

public:

	OrderedRenderingScene() {
		createResources();
		setupScene();
	}

	~OrderedRenderingScene() {
		delete triangle_mesh;
		delete vertex_shader;
		delete fragment_shader;
		delete pipeline;
		for (auto pipeline_instance: pipeline_instances) {
			delete pipeline_instance;
		}

	}

	Entity createTriangle(vec3 position, vec4 color) {

		GraphicPipelineInstanceInfo pipeline_instance_info{};
		pipeline_instance_info.graphic_pipeline = pipeline;
		pipeline_instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;
		GraphicPipelineInstance *pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info);

		pipeline_instances.push_back(pipeline_instance);
		pipeline_instance->setUniform("material", &color);

		Entity triangle_entity = Application::getScene()->createEntity3D();
		MeshRenderer &triangle_renderer = triangle_entity.attach<MeshRenderer>();
		triangle_renderer.mesh = triangle_mesh;
		triangle_renderer.pipeline_instance = pipeline_instance;
		triangle_renderer.ordered = true;
		triangle_entity.get<Transform>().setPosition(position);
		return triangle_entity;

	}

	void setupScene() {
		Entity camera_entity = Application::getScene()->createEntity3D();
		camera_entity.attach<Camera2D>();


		std::vector<Entity> entitities;
		int n = 20;
		for (int i = 0; i < n; ++i) {
			Entity triangle = createTriangle(vec3(0.1, 0, 0),
											 vec4(Random::floatRange(0, 1), Random::floatRange(0, 1), Random::floatRange(0, 1), 1));
			entitities.push_back(triangle);
		}
		for (int i = n - 2; i >= 0; --i) {
			Application::getScene()->setParent(entitities[i], entitities[i + 1]);
		}
		Application::getScene()->print();
	}

	void createResources() {
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
		pipeline_info.fragement_shader = fragment_shader;
		pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_IGNORE_DEPTH_TEST;

		pipeline = Resources::createGraphicPipeline(pipeline_info);

		GraphicPipelineInstanceInfo pipeline_instance_info{};
		pipeline_instance_info.graphic_pipeline = pipeline;
		pipeline_instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;
	}

};