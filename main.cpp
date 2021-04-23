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
    if (Input::getKey(KEY::ESCAPE))
        Application::quit();
}

int main() {
    Application::init();

    //-----------------------CAMERA--------------------------
    //Instantiate an entity with the camera component attached
    auto *camera = Application::scene->instantiate<Camera>();
    camera->entity->transform->translate(vec3(0, 1, 10));
    //Set rendering mode to 3 dimensions
    camera->setRenderMode(PERSPECTIVE);

    //-----------------------TRIANGLE-------------------------
    //Instantiate an entity with the ModelRenderer component attached
    auto *model_renderer = Application::scene->instantiate<ModelRenderer>();
    //Attach custom component
    model_renderer->entity->attach<Rotator>();
    //Assign Mesh
    auto model = new Model();
    model->loadAsync("../../res/models/teapot.obj");

    model_renderer->setModel(*model);

    //-----------------------EVENTS------------------
    Application::onUpdate.subscribe(&onUpdate);
    //-----------------------LOOP--------------------
    Application::run();
    //-----------------------CLEANUP------------------
    Application::onUpdate.unsubscribe(&onUpdate);
    delete model;
    //-----------------------TERMINATE------------------
    Application::terminate();
}