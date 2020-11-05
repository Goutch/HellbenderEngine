#include "HBE.h"

std::vector<Model*> models;
ModelRenderer * model_renderer;
//create a component to rotate the model entity
class Rotator : public Component {
    float turn_per_second = 0.1;
    int index=0;
    Clock c;
    void onAttach() override {
        subscribeUpdate();
    }

    void onUpdate(float delta) override {
        if(c.ms()>1000)
        {
            c.reset();
            index++;
            model_renderer->setModel(*models[index%models.size()]);
            if(!models[index%models.size()]->getMeshes().empty())
            {
                model_renderer->getModel()->getMeshes()[0].second->setColor(vec4(
                        Random::range(0.0f,1.0f),
                        Random::range(0.0f,1.0f),
                        Random::range(0.0f,1.0f),
                        1.0f));
            }
        }
        entity->rotate(M_PI * 2 * delta * turn_per_second, vec3(0, 1, 0));
    }
};

int main() {
    Configs::setWindowTitle("3D view");
    HBE::init();

    //-----------------------CAMERA--------------------------
    //Instanciate an entity with the camera component attached
    Camera *camera = HBE::current_scene->instantiate<Camera>();

    //Set rendering mode to 3 dimensions
    camera->setRenderMode(PERSPECTIVE);
    camera->setFOV(50);
    //set the camera position a in the back
    camera->getEntity()->setPosition(vec3(0, 2, 15));

    //-----------------------MODEL ENTITY------------------
    //Create model entity
    model_renderer = HBE::current_scene->instantiate<ModelRenderer>();
    model_renderer->getEntity()->attach<Rotator>();

    //Create Model resource
    Model *bunny_model = Model::create();
    bunny_model->loadAsync(std::string("../res/models/bunny.obj"));

    Model *teapot_model = Model::create();
    teapot_model->loadAsync(std::string("../res/models/teapot.obj"));

    Model *dragon_model = Model::create();
    dragon_model->loadAsync(std::string("../res/models/dragon.obj"));


    models.push_back(dragon_model);
    models.push_back(bunny_model);
    models.push_back(teapot_model);

    model_renderer->setModel(*bunny_model);
    HBE::run();

    //-----------------------CLEANUP------------------
    //Delete ressources created with Resource.create<>()
    delete teapot_model;
    delete dragon_model;
    delete bunny_model;
    //terminate engine
    HBE::terminate();
}