#include "HBE.h"

using namespace HBE;
static Material *material;
static GraphicPipeline* pipeline;
static Transform* transform;
static Mesh* mesh;
void onUpdate(float delta) {
    //Shut down app if escape key is pressed
    if (Input::getKey(KEY::ESCAPE))
    {
        Application::quit();
    }
}
void onDraw()
{
    Graphics::draw(*transform, *mesh,*material);
}

int main() {

    Application::init();
    //-----------------------SETUP--------------------

    ShaderDB::load("vert","../../res/shaders/VK.vert",SHADER_TYPE::VERTEX);
    ShaderDB::load("frag","../../res/shaders/VK.frag",SHADER_TYPE::FRAGMENT);

   /*vs->load("../../res/shaders/vert.spv", SHADER_TYPE::VERTEX);
    fs->load("../../res/shaders/frag.spv", SHADER_TYPE::FRAGMENT);*/
    GraphicPipeline *pipeline = new GraphicPipeline();
    pipeline->setShaders(ShaderDB::get("vert"),ShaderDB::get("frag"));


    material = new Material();
    material->setPipeline(pipeline);

    transform=new Transform();
    mesh=new Mesh();
    //-----------------------EVENTS------------------
    Application::onUpdate.subscribe(&onUpdate);
    Application::onDraw.subscribe(&onDraw);
    //-----------------------LOOP--------------------
    Application::run();
    //-----------------------CLEANUP------------------
    Application::onUpdate.unsubscribe(&onUpdate);
    Application::onDraw.unsubscribe(&onDraw);
    delete pipeline;
    delete material;
    delete mesh;
    delete transform;
    //-----------------------TERMINATE------------------
    Application::terminate();

}