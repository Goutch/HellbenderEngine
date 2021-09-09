#include "HBE.h"

using namespace HBE;

static Transform *transform;

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
	auto texture = Resources::createInRegistry<Texture>("texture");


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

	/*auto meshRenderer2 = Application::scene->instantiate<MeshRenderer>();
	meshRenderer2->setMaterial(*material);
	meshRenderer2->setMesh(*mesh);
	meshRenderer2->entity->transform->setPosition(vec3(2, 0, 0));
	meshRenderer2->entity->attach<Rotator>();*/

	auto meshRenderer = Application::scene->instantiate<MeshRenderer>();
	meshRenderer->setMaterial(*material);
	meshRenderer->setMesh(*mesh);
	meshRenderer->entity->transform->translate(vec3(1, 0, 0));

	auto mr2 = Application::scene->instantiate<MeshRenderer>();
	mr2->setMaterial(*material);
	mr2->setMesh(*mesh);
	mr2->entity->attach<Rotator>();

	auto camera = Application::scene->instantiate<Camera>();
	camera->setRenderMode(RenderMode::PERSPECTIVE);
	camera->entity->transform->setPosition(vec3(0, 0, 5));

	//unsigned char *texture_data = new unsigned char[4]{255, 255, 255, 255};
	//texture->setData(texture_data, 1, 1, TEXTURE_FORMAT::RGBA8);
	texture->load("../../res/textures/Hellbender_logo.png");

	//camera->entity->attach<CameraController>();
	//-----------------------EVENTS------------------
	Application::onUpdate.subscribe(&onUpdate);

	//-----------------------LOOP--------------------
	Application::run();
	//-----------------------CLEANUP------------------
	Application::onUpdate.unsubscribe(&onUpdate);
	delete transform;
	//delete texture_data;
	//-----------------------TERMINATE------------------

	Application::terminate();

}