#include "HBE.h"

using namespace HBE;

void onAppUpdate(float delta) {
	//Shut down app if escape key is pressed
	if (Input::getKeyDown(KEY::ESCAPE)) {
		Application::quit();
	}
	//change vertical sync if V key is pressed
	if (Input::getKeyDown(KEY::V)) {
		Configs::setVerticalSync(!Configs::getVerticalSync());
	}
}

void init() {
	//-------------------RESOURCES CREATION--------------------------------------
	ShaderInfo frag_info{SHADER_STAGE_FRAGMENT, "../../res/shaders/VK.frag"};
	ShaderInfo vert_info{SHADER_STAGE_VERTEX, "../../res/shaders/VK.vert"};
	auto frag = Resources::createShader(frag_info, "frag");
	auto vert = Resources::createShader(vert_info, "vert");


	std::vector<VertexBindingInfo> binding_infos = {{0, sizeof(vec3) + sizeof(vec2)}};
	GraphicPipelineInfo pipeline_info{};
	pipeline_info.binding_infos = binding_infos.data();
	pipeline_info.binding_info_count = binding_infos.size();
	pipeline_info.fragement_shader = frag;
	pipeline_info.vertex_shader = vert;
	pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_CULL_BACK;
	auto pipeline = Resources::createGraphicPipeline(pipeline_info, "pipeline");

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
	MeshRenderer& cube = cube_entity.attach<MeshRenderer>();
	cube.pipeline = pipeline;
	cube.mesh = cube_mesh;
	cube_entity.get<Transform>().translate(vec3(0, 0, -5.0f));
}

int main() {
	Application::init();

	//-----------------------SETUP--------------------
	Configs::setWindowTitle("Hellbender");

	init();

	//-----------------------EVENTS------------------
	Application::onUpdate.subscribe(&onAppUpdate);
	//-----------------------LOOP--------------------
	Application::run();
	//-----------------------CLEANUP------------------
	Application::onUpdate.unsubscribe(&onAppUpdate);
	//-----------------------TERMINATE------------------

	Application::terminate();

}