#include "HBE.h"
#include "editor/Editor.h"
int main() {
    Configs::setWindowTitle("3D view");

    HBE::init();
    auto teapot=Model::create();
    auto teapot_renderer=HBE::scene->instantiate<ModelRenderer>();
    teapot_renderer->setModel(*teapot);
    teapot->loadAsync("../res/models/teapot.obj");
    auto camera=HBE::scene->instantiate<Camera>();
    camera->getEntity()->setPosition(vec3(0,0,15));

    Editor e;
    e.start();
    HBE::run();

    e.terminate();
    delete teapot;
    HBE::terminate();
}