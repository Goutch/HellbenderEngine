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

struct Vertex {
	vec3 position;
	vec3 color;
	vec2 uv;
};

void init() {
	//-------------------RESOURCES CREATION--------------------------------------
	ShaderInfo frag_info{SHADER_STAGE_FRAGMENT, "../../res/shaders/VK.frag"};
	ShaderInfo vert_info{SHADER_STAGE_VERTEX, "../../res/shaders/VK.vert"};

	auto frag = Resources::createShader(frag_info, "frag");
	auto vert = Resources::createShader(vert_info, "vert");

	std::vector<GLSL_TYPE> vertex_description = std::vector<GLSL_TYPE>{GLSL_TYPE::VEC3F, GLSL_TYPE::VEC2F};

	VertexLayoutInfo layout_info{};
	layout_info.layout_types = vertex_description.data();
	layout_info.layout_types_count = vertex_description.size();
	auto layout = Resources::createVertexLayout(layout_info, "layout");

	GraphicPipelineInfo pipeline_info{};
	pipeline_info.fragement_shader = frag;
	pipeline_info.vertex_shader = vert;
	pipeline_info.vertex_layout = layout;
	pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_NONE;
	auto pipeline = Resources::createGraphicPipeline(pipeline_info, "pipeline");

	auto wall = Texture::load("../../res/textures/wall.png");
	Resources::add("Wall", wall);
	pipeline->setTexture("texSampler", wall);

	auto mesh = Resources::createMesh(MeshInfo{layout}, "mesh");
	Geometry::createCube(*mesh, 1, 1, 1, VERTEX_FLAG_UV);

	//-------------------SCENE CREATION--------------------------------------
	Scene &scene = *Application::getScene();

	Entity camera_entity = scene.createEntity();
	camera_entity.attach<Camera>();
	camera_entity.get<Transform>().setPosition(vec3(0, 0, 5));
	camera_entity.attach<CameraController>();
	scene.setCameraEntity(camera_entity);

	MeshRenderer &cube = scene.createEntity().attach<MeshRenderer>();
	cube.pipeline = pipeline;
	cube.mesh = mesh;
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
	//delete texture_data;
	//-----------------------TERMINATE------------------

	Application::terminate();

}