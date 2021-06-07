#include "HBE.h"

using namespace HBE;
static Material *material;

void onUpdate(float delta) {
    //Shut down app if escape key is pressed
    if (Input::getKey(KEY::ESCAPE))
        Application::quit();


}

int main() {

    Application::init();

    Shader *vs = new Shader();
    Shader *fs = new Shader();

    vs->load("../../res/shaders/VK.vert", SHADER_TYPE::VERTEX);
    fs->load("../../res/shaders/VK.frag", SHADER_TYPE::FRAGMENT);

   /*vs->load("../../res/shaders/vert.spv", SHADER_TYPE::VERTEX);
    fs->load("../../res/shaders/frag.spv", SHADER_TYPE::FRAGMENT);*/
    GraphicPipeline *pipeline = new GraphicPipeline();
    pipeline->setShaders(vs,fs);

    material = new Material();
    material->setPipeline(pipeline);
    Transform transform;
    Mesh mesh;
    Graphics::draw(transform, mesh,*material);
    //-----------------------EVENTS------------------
    Application::onUpdate.subscribe(&onUpdate);
    //-----------------------LOOP--------------------
    Application::run();
    //-----------------------CLEANUP------------------
    Application::onUpdate.unsubscribe(&onUpdate);


    delete vs;
    delete fs;
    delete pipeline;
    delete material;
    //-----------------------TERMINATE------------------
    Application::terminate();

}