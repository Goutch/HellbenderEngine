#pragma once

class MeshRendering {
public:
	static void init() {
		//-------------------RESOURCES CREATION--------------------------------------
		ShaderInfo frag_info{SHADER_STAGE_FRAGMENT, "/shaders/defaults/PositionColor.frag"};
		ShaderInfo vert_info{SHADER_STAGE_VERTEX, "/shaders/defaults/PositionColor.vert"};
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
		attribute_infos[1].size = sizeof(vec4);
		attribute_infos[1].flags = VERTEX_ATTRIBUTE_FLAG_NONE;

		GraphicPipelineInfo pipeline_info{};
		pipeline_info.attribute_infos = attribute_infos.data();
		pipeline_info.attribute_info_count = attribute_infos.size();
		pipeline_info.fragement_shader = frag;
		pipeline_info.vertex_shader = vert;
		pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_NONE;

		auto pipeline = Resources::createGraphicPipeline(pipeline_info, "pipeline");

		GraphicPipelineInstanceInfo material_info{};
		material_info.graphic_pipeline = pipeline;
		auto material = Resources::createGraphicPipelineInstance(material_info, "material");


		MeshInfo mesh_info{};
		mesh_info.attribute_infos = attribute_infos.data();
		mesh_info.attribute_info_count = attribute_infos.size();
		mesh_info.flags = MODEL_FLAG_NONE;


		Mesh *mesh = Resources::createMesh(mesh_info, "mesh");

		vec3 positions[3] = {vec3(-1, -0.5, 0), vec3(0, 1, 0), vec3(1, -0.5, 0)};
		vec4 colors[3] = {vec4(1, 0, 0, 1), vec4(0, 1, 0, 1), vec4(0, 0, 1, 1)};
		mesh->setBuffer(0, positions, 3);
		mesh->setBuffer(1, colors, 3);

		//-------------------SCENE CREATION--------------------------------------
		Scene *scene = new Scene();
		Application::setScene(scene, true);

		Entity camera_entity = scene->createEntity("camera");
		Camera &camera = camera_entity.attach<Camera>();
		camera.render_target = Graphics::getDefaultRenderTarget();
		//camera_entity.attach<CameraController>();
		scene->setCameraEntity(camera_entity);

		auto cube_entity = scene->createEntity("Cube");
		MeshRenderer &renderer = cube_entity.attach<MeshRenderer>();
		cube_entity.get<Transform>().translate(vec3(0, 0, -5));
		cube_entity.get<Transform>().rotate(vec3(0, glm::half_pi<float>(), 0));
		renderer.mesh = mesh;
		renderer.pipelineInstance = material;
		auto cube_entity2 = scene->createEntity("Cube2");
		MeshRenderer &renderer2 = cube_entity2.attach<MeshRenderer>();
		cube_entity2.get<Transform>().translate(vec3(0, 0, 5));
		renderer2.mesh = mesh;
		renderer2.pipelineInstance = material;
	}
};