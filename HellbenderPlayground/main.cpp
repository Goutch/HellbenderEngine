#include "HBE.h"

using namespace HBE;


class Rotator : public Component {
public:
	void onAttach() override {
		//Note: there is no need to unsubscribe in the onDetach function this is handled in the component class.
		subscribeUpdate();
	}

	void onUpdate(float delta) override {
		entity->transform->rotate(delta, vec3(0, 1, 0));

	}
};

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

int main() {
	Application::init();
	//-----------------------SETUP--------------------
	Configs::setWindowTitle("Hellbender");
	auto frag = Resources::createInRegistry<Shader>("frag");
	auto vert = Resources::createInRegistry<Shader>("vert");
	auto pipeline = Resources::createInRegistry<GraphicPipeline>("pipeline");
	auto layout = Resources::createInRegistry<VertexLayout>("layout");
	auto mesh = Resources::createInRegistry<Mesh>("mesh");

	vert->load("../../res/shaders/VK.vert", SHADER_STAGE::VERTEX);
	frag->load("../../res/shaders/VK.frag", SHADER_STAGE::FRAGMENT);
	layout->setLayoutTypes({GLSL_TYPE::VEC3F, GLSL_TYPE::VEC3F, GLSL_TYPE::VEC2F});
	pipeline->setShaders(vert, frag, layout);

	const std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f,  -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{-0.5f, 0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};
	const std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0
	};
	mesh->setVertices(0, (void *) vertices.data(), vertices.size(), layout);
	mesh->setIndices(indices);


	auto mr1 = Application::scene->instantiate<MeshRenderer>();
	mr1->setMaterial(*pipeline);
	mr1->setMesh(*mesh);

	mr1->entity->attach<Rotator>();

	auto mr2 = Application::scene->instantiate<MeshRenderer>();
	mr2->setMaterial(*pipeline);
	mr2->setMesh(*mesh);
	mr2->entity->transform->translate(vec3(2, 0, 0));
	mr2->entity->transform->setParent(mr1->entity->transform);

	auto wall = Resources::createInRegistry<Texture>("Wall");
	wall->load("../../res/textures/wall.png");
	pipeline->setTexture("texSampler",wall);

	auto camera = Application::scene->instantiate<Camera>();
	camera->setRenderMode(RenderMode::PERSPECTIVE);
	camera->entity->transform->setPosition(vec3(0, 0, 5));
	//-----------------------EVENTS------------------
	Application::onUpdate.subscribe(&onUpdate);

	//-----------------------LOOP--------------------
	Application::run();
	//-----------------------CLEANUP------------------
	Application::onUpdate.unsubscribe(&onUpdate);
	//delete texture_data;
	//-----------------------TERMINATE------------------

	Application::terminate();

}