#pragma once
#include "HBE.h"
class LoadModelExemple {
public:
	static void init() {
		//-------------------RESOURCES CREATION--------------------------------------
		ShaderInfo frag_info{SHADER_STAGE_FRAGMENT, "/shaders/defaults/PositionNormal.frag"};
		ShaderInfo vert_info{SHADER_STAGE_VERTEX, "/shaders/defaults/PositionNormal.vert"};
		auto frag = Resources::createShader(frag_info, "frag");
		auto vert = Resources::createShader(vert_info, "vert");

		std::vector<VertexBindingInfo> binding_infos;
		//vertex binding
		binding_infos.emplace_back();
		binding_infos[0].binding = 0;
		binding_infos[0].size = sizeof(vec3);
		binding_infos[0].flags = VERTEX_BINDING_FLAG_NONE;
		/*binding_infos.emplace_back();
		binding_infos[2].binding = 2;
		binding_infos[2].size = sizeof(vec3);
		binding_infos[2].flags = VERTEX_BINDING_FLAG_NONE;*/
		binding_infos.emplace_back();
		binding_infos[1].binding = 1;
		binding_infos[1].size = sizeof(vec3);
		binding_infos[1].flags = VERTEX_BINDING_FLAG_NONE;

		std::vector<VertexBindingInfo> ground_binding_infos;
		//vertex binding
		ground_binding_infos.emplace_back();
		ground_binding_infos[0].binding = 0;
		ground_binding_infos[0].size = sizeof(vec3) * 2;
		ground_binding_infos[0].flags = VERTEX_BINDING_FLAG_NONE;

		GraphicPipelineInfo pipeline_info{};
		pipeline_info.binding_infos = binding_infos.data();
		pipeline_info.binding_info_count = binding_infos.size();
		pipeline_info.fragement_shader = frag;
		pipeline_info.vertex_shader = vert;
		pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_CULL_BACK;

		auto model_pipeline = Resources::createGraphicPipeline(pipeline_info, "DEFAULT_MODEL_PIPELINE");

		MaterialInfo model_material_info{};
		model_material_info.graphic_pipeline = model_pipeline;
		Resources::createMaterial(model_material_info, "DEFAULT_MODEL_MATERIAL");

		pipeline_info.binding_infos = ground_binding_infos.data();
		pipeline_info.binding_info_count = ground_binding_infos.size();

		auto ground_pipeline = Resources::createGraphicPipeline(pipeline_info, "GROUND_PIPELINE");

		MaterialInfo ground_material_info{};
		ground_material_info.graphic_pipeline = ground_pipeline;
		auto ground_material = Resources::createMaterial(ground_material_info, "GROUND_MATERIAL");


		vec4 ground_material_color = vec4(0.2, 0.7, 0., 1.);
		ground_material->setUniform("material", &ground_material_color);


		ModelInfo model_info{};
		model_info.path = "/models/teapot.gltf";
		model_info.flags = MODEL_FLAG_NONE;

		Model *teapot_model = Resources::createModel(model_info, "teapot");

		model_info.path = "/models/dragon.gltf";
		Model *dragon_model = Resources::createModel(model_info, "dragon");


		MeshInfo mesh_info{};
		mesh_info.binding_infos = ground_binding_infos.data();
		mesh_info.binding_info_count = ground_binding_infos.size();
		mesh_info.flags = MESH_FLAG_NONE;
		Mesh *ground_mesh = Resources::createMesh(mesh_info, "ground");
		Geometry::createQuad(*ground_mesh, 20, 20, VERTEX_FLAG_NORMAL);

		//-------------------SCENE CREATION--------------------------------------
		Scene *scene = new Scene();
		Application::setScene(scene, true);

		Entity camera_entity = scene->createEntity("camera");
		Camera &camera = camera_entity.attach<Camera>();
		camera.render_target = Graphics::getDefaultRenderTarget();
		scene->setCameraEntity(camera_entity);

		auto ground_entity = scene->createEntity("ground");
		MeshRenderer &ground_renderer = ground_entity.attach<MeshRenderer>();
		ground_renderer.mesh = ground_mesh;
		ground_renderer.material = ground_material;
		ground_entity.get<Transform>().rotate(vec3(- M_PI / 2, 0, 0));
		ground_entity.get<Transform>().translate(vec3(0, -0.7, 0));


		auto teapot = scene->createEntity("teapot");
		ModelRenderer &teapot_renderer = teapot.attach<ModelRenderer>();
		teapot.get<Transform>().translate(vec3(0, 0, -5));
		teapot_renderer.model = teapot_model;
		teapot.get<Transform>().setScale(vec3(0.01f));


		auto dragon = scene->createEntity("Dragon");
		ModelRenderer &dragon_renderer = dragon.attach<ModelRenderer>();
		dragon.get<Transform>().translate(vec3(0, 0, 5));
		dragon_renderer.model = dragon_model;
	}
};
