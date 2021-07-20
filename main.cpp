#include "HBE.h"

using namespace HBE;

static Transform *transform;


void onUpdate(float delta) {
    //Shut down app if escape key is pressed
    if (Input::getKey(KEY::ESCAPE)) {
        Application::quit();
    }
}

void onDraw() {
    Graphics::draw(*transform, *Resources::get<Mesh>("mesh"), *Resources::get<Material>("material"));
}

struct Vertex {
    vec2 position;
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
    auto *material = Resources::createInRegistry<Material>("material");

    vert->load("../../res/shaders/VK.vert", SHADER_TYPE::VERTEX);
    frag->load("../../res/shaders/VK.frag", SHADER_TYPE::FRAGMENT);
    layout->setLayoutTypes({GLSL_TYPE::VEC2F, GLSL_TYPE::VEC3F});
    pipeline->setShaders(vert, frag, layout);

    const std::vector<Vertex> vertices = {
            {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };
    mesh->setVertices(0, (void *) vertices.data(), vertices.size(), layout);
    material->setPipeline(pipeline);
    transform = new Transform();
    //-----------------------EVENTS------------------
    Application::onUpdate.subscribe(&onUpdate);
    Application::onDraw.subscribe(&onDraw);
    //-----------------------LOOP--------------------
    Application::run();
    //-----------------------CLEANUP------------------
    Application::onUpdate.unsubscribe(&onUpdate);
    Application::onDraw.unsubscribe(&onDraw);
    delete transform;
    //-----------------------TERMINATE------------------

    Application::terminate();

}