#include "HBE.h"
#include "editor/Editor.h"

int main() {

    Configs::setWindowTitle("3D view");
    Application::init();
    auto teapot = Model::create();
    auto teapot_renderer = Application::scene->instantiate<ModelRenderer>("Teapot");
    auto teapot_child = Application::scene->instantiate(teapot_renderer->getEntity());
    teapot_renderer->setModel(*teapot);
    teapot->loadAsync("../../res/models/teapot.obj");
    auto camera = Application::scene->instantiate<Camera>("Camera");
    camera->getEntity()->setPosition(vec3(0, 0, 15));

    Editor e;
    e.start();
    Application::run();

    e.terminate();
    delete teapot;
    Application::terminate();
}