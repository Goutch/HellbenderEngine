#include "HBE.h"

using namespace HBE;

class CubeRendering {
public:
	static void init() {

		ShaderInfo shader_info{};
		shader_info.path = "shaders/defaults/Position.frag";
		shader_info.stage = SHADER_STAGE_FRAGMENT;
		Shader *frag = Resources::createShader(shader_info, "frag");

		shader_info.path = "shaders/defaults/Position.vert";
		shader_info.stage = SHADER_STAGE_VERTEX;
		Shader *vert = Resources::createShader(shader_info, "vert");

		GraphicPipelineInfo pipeline_info{};
		pipeline_info.attribute_info_count = 1;
		pipeline_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
		pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_CULL_BACK;
		pipeline_info.fragement_shader = frag;
		pipeline_info.vertex_shader = vert;
		GraphicPipeline *graphicPipeline = Resources::createGraphicPipeline(pipeline_info, "pipeline");

		GraphicPipelineInstanceInfo pipeline_instance_info{};
		pipeline_instance_info.graphic_pipeline = graphicPipeline;
		GraphicPipelineInstance *pipeline_instance = Resources::createGraphicPipelineInstance(pipeline_instance_info, "pipelineInstance");

		MeshInfo mesh_info{};
		mesh_info.attribute_infos = &VERTEX_ATTRIBUTE_INFO_POSITION3D;
		mesh_info.attribute_info_count = 1;

		Mesh *mesh = Resources::createMesh(mesh_info, "cube");
		Geometry::createQuad(*mesh, 1, 1, VERTEX_FLAG_NONE);

		Scene &scene = *Application::getScene();

		Entity cube_entity = scene.createEntity3D();
		MeshRenderer &cube_renderer = cube_entity.attach<MeshRenderer>();
		cube_renderer.mesh = mesh;
		cube_renderer.pipelineInstance = pipeline_instance;
		cube_entity.get<Transform>().translate(vec3(0, 0, -5));
		cube_entity.get<Transform>().rotate(vec3(-M_PI / 2, 0, 0));
		Entity camera_entity = scene.createEntity3D();
		camera_entity.attach<Camera>();
		scene.setCameraEntity(camera_entity);

		vec4 c = vec4(1, 0, 0, 1);
		pipeline_instance->setUniform("material", &c);
	}
};