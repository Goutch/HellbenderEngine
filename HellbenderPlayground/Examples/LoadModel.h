#pragma once

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
		binding_infos[0].size = sizeof(vec3) ;
		binding_infos[0].flags = VERTEX_BINDING_FLAG_NONE;
		/*binding_infos.emplace_back();
		binding_infos[2].binding = 2;
		binding_infos[2].size = sizeof(vec3);
		binding_infos[2].flags = VERTEX_BINDING_FLAG_NONE;*/
		binding_infos.emplace_back();
		binding_infos[1].binding = 1;
		binding_infos[1].size = sizeof(vec3) ;
		binding_infos[1].flags = VERTEX_BINDING_FLAG_NONE;

		GraphicPipelineInfo pipeline_info{};
		pipeline_info.binding_infos = binding_infos.data();
		pipeline_info.binding_info_count = binding_infos.size();
		pipeline_info.fragement_shader = frag;
		pipeline_info.vertex_shader = vert;
		pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_NONE;

		Resources::createGraphicPipeline(pipeline_info, "DEFAULT_MODEL_PIPELINE");


		ModelInfo model_info{};
		model_info.path = "/models/teapot.gltf";
		model_info.flags = MODEL_FLAG_NONE;

		Model *model = Resources::createModel(model_info, "model");

		//-------------------SCENE CREATION--------------------------------------
		Scene *scene = new Scene();
		Application::setScene(scene, true);

		Entity camera_entity = scene->createEntity("camera");
		Camera &camera = camera_entity.attach<Camera>();
		camera.render_target = Graphics::getDefaultRenderTarget();
		//camera_entity.attach<CameraController>();
		scene->setCameraEntity(camera_entity);

		auto cube_entity = scene->createEntity("Cube");
		ModelRenderer &renderer = cube_entity.attach<ModelRenderer>();
		cube_entity.get<Transform>().translate(vec3(0, 0, -5));
		renderer.model = model;
		cube_entity.get<Transform>().setScale(vec3(0.01f));
		/*auto cube_entity2 = scene->createEntity("Cube2");
		ModelRenderer &renderer2 = cube_entity2.attach<ModelRenderer>();
		cube_entity2.get<Transform>().translate(vec3(0, 0, 5));
		renderer2.model = model;*/
	}
};
