
#include "RaytracingExemple.h"
#include "HBE.h"

using namespace HBE;

RaytracingExemple::RaytracingExemple() {
//-------------------RESOURCES CREATION--------------------------------------
	ShaderInfo frag_info{SHADER_STAGE_FRAGMENT, "/shaders/defaults/VK.frag"};
	ShaderInfo vert_info{SHADER_STAGE_VERTEX, "/shaders/defaults/VK.vert"};
	auto frag = Resources::createShader(frag_info, "frag");
	auto vert = Resources::createShader(vert_info, "vert");


	std::vector<VertexBindingInfo> binding_infos = {{0, sizeof(vec3) + sizeof(vec2)}};
	GraphicPipelineInfo pipeline_info{};
	pipeline_info.binding_infos = binding_infos.data();
	pipeline_info.binding_info_count = binding_infos.size();
	pipeline_info.fragement_shader = frag;
	pipeline_info.vertex_shader = vert;
	pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_NONE;
	auto pipeline = Resources::createGraphicPipeline(pipeline_info, "pipeline");
	vec4 color = vec4(0.7, 0.3, 0.1, 1);
	pipeline->setUniform("material", &color);


	MeshInfo mesh_info{};
	mesh_info.binding_infos = binding_infos.data();
	mesh_info.binding_info_count = binding_infos.size();
	mesh_info.flags = MESH_FLAG_NONE;
	auto cube_mesh = Resources::createMesh(mesh_info, "mesh");


	Geometry::createCube(*cube_mesh, 1, 1, 1, VERTEX_FLAG_UV);

	//-------------------SCENE CREATION--------------------------------------
	Scene &scene = *(new Scene());
	Application::setScene(&scene, true);

	Entity camera_entity = scene.createEntity("camera");
	Camera &camera = camera_entity.attach<Camera>();
	camera.render_target = Graphics::getDefaultRenderTarget();

	scene.setCameraEntity(camera_entity);

	Entity cube_entity = scene.createEntity();
	MeshRenderer &cube = cube_entity.attach<MeshRenderer>();
	cube.pipeline = pipeline;
	cube.mesh = cube_mesh;
	cube_entity.get<Transform>().translate(vec3(0, 0, -5.0f));

}