#include <core/scene/systems/RotatorSystem.h>
#include "HBE.h"

using namespace HBE;


void onAppUpdate(float delta) {
	//Shut down app if escape key is pressed
	if (Input::getKeyDown(KEY::ESCAPE)) {
		Application::quit();
	}
	if (Input::getKeyDown(KEY::V)) {
		Configs::setVerticalSync(!Configs::getVerticalSync());
	}
}

void init() {
	//-------------------RESOURCES CREATION--------------------------------------
	ShaderInfo frag_info{SHADER_STAGE_FRAGMENT, "../../res/shaders/VK.frag"};
	ShaderInfo vert_info{SHADER_STAGE_VERTEX, "../../res/shaders/VK.vert"};
	ShaderInfo instanced_vert_info{SHADER_STAGE_VERTEX, "../../res/shaders/VK_instanced.vert"};
	auto frag = Resources::createShader(frag_info, "frag");
	auto vert = Resources::createShader(vert_info, "vert");
	auto instanced_vert = Resources::createShader(instanced_vert_info, "instance_vert");

	std::vector<VertexBindingInfo> binding_infos = {{0, sizeof(vec3) + sizeof(vec2)}};
	std::vector<VertexBindingInfo> binding_infos_instanced = {{0, sizeof(vec3) + sizeof(vec2)},
															  {1, sizeof(mat4),
																	  VERTEX_BINDING_FLAG_PER_INSTANCE
																	  | VERTEX_BINDING_FLAG_FAST_WRITE
																	  | VERTEX_BINDING_FLAG_MULTIPLE_BUFFERS}};

	GraphicPipelineInfo pipeline_info{};
	pipeline_info.fragement_shader = frag;
	pipeline_info.vertex_shader = vert;
	pipeline_info.binding_info_count = binding_infos.size();
	pipeline_info.binding_infos = binding_infos.data();
	pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_NONE;
	auto pipeline = Resources::createGraphicPipeline(pipeline_info, "pipeline");

	GraphicPipelineInfo pipeline_info_instanced{};
	pipeline_info_instanced.fragement_shader = frag;
	pipeline_info_instanced.vertex_shader = instanced_vert;
	pipeline_info_instanced.binding_info_count = binding_infos_instanced.size();
	pipeline_info_instanced.binding_infos = binding_infos_instanced.data();
	pipeline_info_instanced.flags = GRAPHIC_PIPELINE_FLAG_CULL_BACK;
	auto pipeline_instanced = Resources::createGraphicPipeline(pipeline_info_instanced, "pipeline_instanced");

	auto mesh = Resources::createMesh(MeshInfo{binding_infos.data(), binding_infos.size()}, "mesh");
	auto instanced_mesh = Resources::createMesh(MeshInfo{binding_infos_instanced.data(), binding_infos_instanced.size()}, "mesh_instanced");


	Geometry::createCube(*mesh, 1, 1, 1, VERTEX_FLAG_UV);
	Geometry::createCube(*instanced_mesh, 1, 1, 1, VERTEX_FLAG_UV);




	//-------------------SCENE CREATION--------------------------------------
	Scene &scene = *(new Scene());
	Application::setScene(&scene, true);

	scene.addSystem(new RotatorSystem(&scene));

	Entity camera_entity = scene.createEntity("camera");
	Camera &camera = camera_entity.attach<Camera>();
	camera.render_target = Graphics::getDefaultRenderTarget();
	camera.calculateProjection();
	//camera_entity.attach<CameraController>();
	camera_entity.get<Transform>().setPosition(vec3(0, 0, 0));
	scene.setCameraEntity(camera_entity);

	Random random;
	for (int i = -1; i < 1; ++i) {
		for (int j = -1; j < 1; ++j) {
			for (int k = -1; k < 1; ++k) {
				Entity cube_entity = scene.createEntity();
				auto &renderer = cube_entity.attach<MeshRenderer>();
				renderer.pipeline = pipeline;
				renderer.mesh = mesh;

				cube_entity.get<Transform>().translate(vec3(i, j, k) * 5.0f);
				/*Rotator &rotator = cube_entity.attach<Rotator>();
				rotator.rotate_speed = random.floatRange(M_PI, 5.0f);
				rotator.angle = vec3(random.floatRange(-M_PI, M_PI),
									 random.floatRange(-M_PI, M_PI),
									 random.floatRange(-M_PI, M_PI));*/
			}
		}
	}

}

int main() {
	Application::init();
	Log::setFlags(Log::getFlags());
	//-----------------------SETUP--------------------
	Configs::setWindowTitle("Hellbender");

	init();

	//-----------------------EVENTS------------------
	Application::onUpdate.subscribe(&onAppUpdate);
	//-----------------------LOOP--------------------
	Application::run();
	//-----------------------CLEANUP------------------
	Application::onUpdate.unsubscribe(&onAppUpdate);
	//delete texture_data;
	//-----------------------TERMINATE------------------

	Application::terminate();

}