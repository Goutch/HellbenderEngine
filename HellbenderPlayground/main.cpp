#include "HBE.h"

using namespace HBE;


void onUpdate(float delta) {
	//Shut down app if escape key is pressed
	if (Input::getKeyDown(KEY::ESCAPE)) {
		Application::quit();
	}
	if (Input::getKeyDown(KEY::V)) {
		Log::debug("setVsync");
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
	pipeline_info.flags = GRAPHIC_PIPELINE_CULL_BACK;
	auto pipeline = Resources::createGraphicPipeline(pipeline_info, "pipeline");

	auto wall = Texture::load("../../res/textures/wall.png");
	Resources::add("Wall", wall);
	pipeline->setTexture("texSampler", wall);

	auto mesh = Resources::createMesh(MeshInfo{layout}, "mesh");
	Geometry::createCube(*mesh, 1, 1, 1, VERTEX_FLAG_UV);


	//-------------------SCENE CREATION--------------------------------------

	//


/*
	auto mr1 = Application::scene->instantiate<MeshRenderer>();
	mr1->setMaterial(*pipeline);
	mr1->setMesh(*mesh);

	mr1->entity->attach<Rotator>();

	auto mr2 = Application::scene->instantiate<MeshRenderer>();
	mr2->setMaterial(*pipeline);
	mr2->setMesh(*mesh);
	mr2->entity->transform->translate(vec3(2, 0, 0));
	mr2->entity->transform->setParent(mr1->entity->transform);

	auto camera = Application::scene->instantiate<Camera>();
	camera->setRenderMode(RENDER_MODE::PERSPECTIVE);
	camera->entity->transform->setPosition(vec3(0, 0, 5));*/
}

int main() {
    Scene scene;
    Entity entity=scene.createEntity();
    entity.attach<Transform>();
    entity.attach<Camera>();
    auto group = scene.group<Transform, Camera>();
    for (EntityHandle entity_handle:group) {
        group.get<Transform>(entity_handle);
        group.get<Camera>(entity_handle);
    }

    /*
	Application::init();
	//-----------------------SETUP--------------------
	Configs::setWindowTitle("Hellbender");


	init();

	//-----------------------EVENTS------------------
	Application::onUpdate.subscribe(&onUpdate);

	//-----------------------LOOP--------------------
	Application::run();
	//-----------------------CLEANUP------------------
	Application::onUpdate.unsubscribe(&onUpdate);
	//delete texture_data;
	//-----------------------TERMINATE------------------

	Application::terminate();*/

}