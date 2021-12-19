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
	ShaderInfo vert_info{SHADER_STAGE_VERTEX, "../../res/shaders/VK_instanced.vert"};
	auto frag = Resources::createShader(frag_info, "frag");
	auto vert = Resources::createShader(vert_info, "vert");


	std::vector<VertexBindingInfo> binding_infos = {{0, sizeof(vec3) + sizeof(vec2)},    //vertex binding
													{1, sizeof(mat4),                            // instance binding
															VERTEX_BINDING_FLAG_PER_INSTANCE
															| VERTEX_BINDING_FLAG_FAST_WRITE
															| VERTEX_BINDING_FLAG_MULTIPLE_BUFFERS}};
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
	auto mesh = Resources::createMesh(mesh_info, "mesh");

	Geometry::createCube(*mesh, 1, 1, 1, VERTEX_FLAG_UV);

	//-------------------SCENE CREATION--------------------------------------
	Scene &scene = *(new Scene());
	Application::setScene(&scene, true);

	Entity camera_entity = scene.createEntity("camera");
	Camera &camera = camera_entity.attach<Camera>();
	camera.render_target = Graphics::getDefaultRenderTarget();
	camera_entity.attach<CameraController>();

	scene.setCameraEntity(camera_entity);

	int32_t range = 5; //10*10*10 = 1000 cubes
	for (int i = -range; i < range; ++i) {
		for (int j = -range; j < range; ++j) {
			for (int k = -range; k < range; ++k) {
				Entity cube_entity = scene.createEntity();
				auto &renderer = cube_entity.attach<InstancedRenderer>();
				renderer.pipeline = pipeline;
				renderer.mesh = mesh;

				cube_entity.get<Transform>().translate(vec3(i, j, k) * 2.0f);
			}
		}
	}

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