#include "HBE.h"

using namespace HBE;

class CubeScene {
	Mesh *cube_mesh;
	Shader *fragment_shader;
	Shader *vertex_shader;
	GraphicPipeline *pipeline;
	GraphicPipelineInstance *pipeline_instance;
public:

	CubeScene() {
		createResources();
		setupScene();
	}

	~CubeScene() {
		delete cube_mesh;
		delete fragment_shader;
		delete vertex_shader;
		delete pipeline;
		delete pipeline_instance;
	}

	void createResources() {
		ShaderInfo shader_info{};
		shader_info.path = "shaders/defaults/Position.frag";
		shader_info.stage = SHADER_STAGE_FRAGMENT;
		fragment_shader = Resources::createShader(shader_info);

		shader_info.path = "shaders/defaults/Position.vert";
		shader_info.stage = SHADER_STAGE_VERTEX;
		vertex_shader = Resources::createShader(shader_info);

		GraphicPipelineInfo pipeline_info{};
		pipeline_info.attribute_info_count = 1;
		pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
		pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_CULL_BACK;
		pipeline_info.fragement_shader = fragment_shader;
		pipeline_info.vertex_shader = vertex_shader;

		pipeline = Resources::createGraphicPipeline(pipeline_info);

		GraphicPipelineInstanceInfo pipeline_instance_info{};
		pipeline_instance_info.graphic_pipeline = pipeline;
		pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info);

		MeshInfo mesh_info{};
		mesh_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
		mesh_info.attribute_info_count = 1;

		cube_mesh = Resources::createMesh(mesh_info);
		Geometry::createCube(*cube_mesh, 1, 1, 1, VERTEX_FLAG_NONE);
	}

	void setupScene() {
		Scene &scene = *Application::getScene();

		Entity cube_entity = scene.createEntity3D();
		MeshRenderer &cube_renderer = cube_entity.attach<MeshRenderer>();
		cube_renderer.mesh = cube_mesh;
		cube_renderer.pipeline_instance = pipeline_instance;
		cube_entity.get<Transform>().translate(vec3(0, 0, -5));
		cube_entity.get<Transform>().rotate(vec3(-M_PI / 2, 0, 0));

		Entity camera_entity = scene.createEntity3D();
		camera_entity.attach<Camera>();

		vec4 c = vec4(1, 0, 0, 1);
		pipeline_instance->setUniform("material", &c);
	}

};