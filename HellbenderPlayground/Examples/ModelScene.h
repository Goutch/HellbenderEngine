#pragma once

#include "HBE.h"
#include "FPSCounter.h"

class ModelScene {
private :
	Entity entitites[2];
public:

	void update(float delta) {
		for (int i = 0; i < 2; i++) {
			entitites[i].get<Transform>().rotate(glm::vec3(0, delta, 0));
		}
	}

	ModelScene() {

		//-------------------RESOURCES CREATION--------------------------------------
		ShaderInfo frag_info{SHADER_STAGE_FRAGMENT, "/shaders/defaults/PositionNormal.frag"};
		ShaderInfo vert_info{SHADER_STAGE_VERTEX, "/shaders/defaults/PositionNormal.vert"};
		auto frag = Resources::createShader(frag_info, "frag");
		auto vert = Resources::createShader(vert_info, "vert");

		std::vector<VertexAttributeInfo> attribute_infos;
		//vertex binding
		attribute_infos.emplace_back();
		attribute_infos[0].binding = 0;
		attribute_infos[0].size = sizeof(vec3);
		attribute_infos[0].flags = VERTEX_ATTRIBUTE_FLAG_NONE;
		attribute_infos.emplace_back();
		attribute_infos[1].binding = 1;
		attribute_infos[1].size = sizeof(vec3);
		attribute_infos[1].flags = VERTEX_ATTRIBUTE_FLAG_NONE;

		GraphicPipelineInfo pipeline_info{};
		pipeline_info.attribute_infos = attribute_infos.data();
		pipeline_info.attribute_info_count = attribute_infos.size();
		pipeline_info.fragement_shader = frag;
		pipeline_info.vertex_shader = vert;
		pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_FRONT_COUNTER_CLOCKWISE | GRAPHIC_PIPELINE_FLAG_CULL_BACK; //gltf primitives are counter clockwise


		auto model_pipeline = Resources::createGraphicPipeline(pipeline_info, "DEFAULT_MODEL_PIPELINE");

		GraphicPipelineInstanceInfo model_material_info{};
		model_material_info.graphic_pipeline = model_pipeline;
		//THIS IS A WORKAROUND :( setting pipeline to DEFAULT_MODEL_MATERIAL to get it when creating the model
		Resources::createGraphicPipelineInstance(model_material_info, "DEFAULT_MODEL_MATERIAL");

		pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D_NORMAL_INTERLEAVED;
		pipeline_info.attribute_info_count = 1;

		ModelInfo model_info{};
		model_info.path = "/models/teapot.gltf";
		model_info.flags = MODEL_FLAG_NONE;

		Model *teapot_model = Resources::createModel(model_info, "teapot");

		model_info.path = "/models/dragon.gltf";
		Model *dragon_model = Resources::createModel(model_info, "dragon");

		//-------------------SCENE CREATION--------------------------------------
		Scene *scene = new Scene();
		Application::setScene(scene, true);

		Entity camera_entity = scene->createEntity3D();
		Camera &camera = camera_entity.attach<Camera>();
		camera.render_target = Graphics::getDefaultRenderTarget();
		scene->setCameraEntity(camera_entity);

		auto teapot = scene->createEntity3D();
		ModelRenderer &teapot_renderer = teapot.attach<ModelRenderer>();
		teapot.get<Transform>().translate(vec3(2.5, 0, -5));
		teapot.get<Transform>().setScale(vec3(0.1));
		teapot_renderer.model = teapot_model;


		auto dragon = scene->createEntity3D();
		ModelRenderer &dragon_renderer = dragon.attach<ModelRenderer>();
		dragon.get<Transform>().translate(vec3(-2.5, 0, -5));
		dragon_renderer.model = dragon_model;

		entitites[0] = teapot;
		entitites[1] = dragon;

		Application::onUpdate.subscribe(this,&ModelScene::update);
	}
};

