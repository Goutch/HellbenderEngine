#pragma once

#include "HBE.h"

using namespace HBE;

class TriangleScene : public Scene {
	Mesh *triangle_mesh;

	Shader *vertex_shader;
	Shader *fragment_shader;
	GraphicPipeline *pipeline;
	GraphicPipelineInstance *pipeline_instance;

public:

	TriangleScene() :Scene() {
		createResources();
		Entity e = createEntity3D();
		attach<Transform>(e.getHandle());
		setupScene();
	}

	~TriangleScene() {
		delete triangle_mesh;
		delete vertex_shader;
		delete fragment_shader;
		delete pipeline;
		delete pipeline_instance;
	}

	void setupScene() {
		Entity camera_entity = createEntity3D();
		camera_entity.attach<Transform>();
		camera_entity.attach<Camera2D>().layer_mask = 2;
		setCameraEntity(camera_entity);

		Entity triangle_entity = createEntity3D();
		MeshRenderer &triangle_renderer = triangle_entity.attach<MeshRenderer>();
		triangle_renderer.mesh = triangle_mesh;
		triangle_renderer.layer = 2;
		triangle_renderer.pipeline_instance = pipeline_instance;
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
		triangle_mesh->setBuffer(0, vertices, sizeof(vertices));


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
		pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_NONE;

		pipeline = Resources::createGraphicPipeline(pipeline_info);

		GraphicPipelineInstanceInfo pipeline_instance_info{};
		pipeline_instance_info.graphic_pipeline = pipeline;
		pipeline_instance_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;

		pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info);

		vec4 color = {1, 0, 0, 1};
		pipeline_instance->setUniform("material", &color);
	}

};