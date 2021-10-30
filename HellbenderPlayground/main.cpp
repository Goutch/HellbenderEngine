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

void init()
{
	ShaderInfo frag_info{SHADER_STAGE_FRAGMENT, "../../res/shaders/VK.frag"};
	ShaderInfo vert_info{SHADER_STAGE_VERTEX, "../../res/shaders/VK.vert"};


	auto frag = Resources::createShader(frag_info, "frag");
	auto vert = Resources::createShader(vert_info, "vert");


	VertexLayoutInfo layout_info{ std::vector<GLSL_TYPE>({GLSL_TYPE::VEC3F, GLSL_TYPE::VEC3F, GLSL_TYPE::VEC2F})};
	auto layout = Resources::createVertexLayout(layout_info, "layout");

	GraphicPipelineInfo pipeline_info{};
	pipeline_info.fragement_shader=frag;
	pipeline_info.vertex_shader=vert;
	pipeline_info.vertex_layout=layout;
	pipeline_info.flags=GRAPHIC_PIPELINE_FLAG_NONE;
	auto pipeline = Resources::createGraphicPipeline(pipeline_info,"pipeline");


	RenderTargetInfo render_target_info{500,500};
	RenderTarget* render_target=Resources::createRenderTarget(render_target_info,"render_target");


	auto mesh = Resources::createMesh(MeshInfo{},"mesh");
	const std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f, 0.0f},  {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f,  -0.5f, 0.0f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f,  0.5f,  0.0f},  {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{-0.5f, 0.5f,  0.0f},  {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

			{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f,  -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f,  0.5f,  -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{-0.5f, 0.5f,  -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
	};
	const std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
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

	auto wall = Texture::load("../../res/textures/wall.png");
	Resources::add("Wall",wall);

	pipeline->setTexture("texSampler", wall);

	auto camera = Application::scene->instantiate<Camera>();
	camera->setRenderMode(RENDER_MODE::PERSPECTIVE);
	camera->entity->transform->setPosition(vec3(0, 0, 5));

	/*Graphics::beginFrame();
	Graphics::draw(*mr1->entity->transform,*mesh,*pipeline);
	Graphics::render(render_target,camera->getProjectionMatrix(),camera->getViewMatrix());
	Graphics::endFrame();*/
}
int main() {
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

	Application::terminate();

}