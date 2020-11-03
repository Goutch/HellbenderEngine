#include "HBE.h"

//create a component to rotate the model entity
class Rotator : public Component {
    float turn_per_second=0.1;
    void onAttach() override
    {
        subscribeUpdate();
    }
    void onUpdate(float delta) override {
        entity->rotate(M_PI*2*delta*turn_per_second, vec3(0, 1, 0));
    }
};

int main() {
    HBE::init();

    //-----------------------CAMERA--------------------------
    //Instanciate an entity with the camera component attached
    Camera *camera = HBE::current_scene->instantiate<Camera>();

    //Set rendering mode to 3 dimensions
    camera->setRenderMode(PERSPECTIVE);
    camera->setFOV(70);
    //set the camera position a in the back
    camera->getEntity()->setPosition(vec3(0, 2, 5));

    //-----------------------MODEL ENTITY------------------
    //Create model entity
    ModelRenderer *model_renderer = HBE::current_scene->instantiate<ModelRenderer>();
    model_renderer->getEntity()->attach<Rotator>();

    //Create Model resource
    auto model = Model::create()->load(std::string("../teapot.obj"));
    model_renderer->setModel(*model);
    HBE::run();

    //-----------------------CLEANUP------------------
    //Delete ressources created with Resource.create<>()
    delete model;

    //terminate engine
    HBE::terminate();
}