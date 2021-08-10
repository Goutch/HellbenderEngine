#include "HBE.h"

using namespace HBE;

static Transform *transform;


void onUpdate(float delta) {
	//Shut down app if escape key is pressed
	if (Input::getKey(KEY::ESCAPE)) {
		Application::quit();
	}
}

struct Vertex {
	vec3 position;
	vec3 color;
};

int main() {

	Application::init();
	//-----------------------SETUP--------------------

	auto frag = Resources::createInRegistry<Shader>("frag");
	auto vert = Resources::createInRegistry<Shader>("vert");
	auto pipeline = Resources::createInRegistry<GraphicPipeline>("pipeline");
	auto layout = Resources::createInRegistry<VertexLayout>("layout");
	auto mesh = Resources::createInRegistry<Mesh>("mesh");
	auto material = Resources::createInRegistry<Material>("material");

	vert->load("../../res/shaders/VK.vert", SHADER_STAGE::VERTEX);
	frag->load("../../res/shaders/VK.frag", SHADER_STAGE::FRAGMENT);
	layout->setLayoutTypes({GLSL_TYPE::VEC3F, GLSL_TYPE::VEC3F});
	pipeline->setShaders(vert, frag, layout);

	const std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f,  -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}}
	};
	const std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0
	};
	mesh->setVertices(0, (void *) vertices.data(), vertices.size(), layout);
	mesh->setIndices(indices);
	material->setPipeline(pipeline);

	auto meshRenderer = Application::scene->instantiate<MeshRenderer>();
	meshRenderer->setMaterial(*material);
	meshRenderer->setMesh(*mesh);

	auto camera = Application::scene->instantiate<Camera>();
	camera->setRenderMode(RenderMode::PERSPECTIVE);
	camera->entity->transform->setPosition(vec3(0, 0, 5));
	camera->entity->attach<CameraController>();
	//-----------------------EVENTS------------------
	Application::onUpdate.subscribe(&onUpdate);

	//-----------------------LOOP--------------------
	Application::run();
	//-----------------------CLEANUP------------------
	Application::onUpdate.unsubscribe(&onUpdate);
	delete transform;
	//-----------------------TERMINATE------------------

	Application::terminate();

}