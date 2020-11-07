#include "HBE.h"
#include "editor/Editor.h"
int main() {
    Configs::setWindowTitle("3D view");

    HBE::init();
    auto teapot=Model::create();
    auto teapot_renderer=HBE::current_scene->instantiate<ModelRenderer>();
    teapot_renderer->setModel(*teapot);
    teapot->loadAsync("../res/models/teapot.obj");
    auto camera=HBE::current_scene->instantiate<Camera>();
    camera->getEntity()->setPosition(vec3(0,0,15));

    Editor e;
    e.start();
    HBE::run();

    e.terminate();
    delete teapot;
    HBE::terminate();
}