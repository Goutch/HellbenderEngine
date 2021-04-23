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

    //Set rendering mode to 2 dimensions
    camera->setRenderMode(ORTHOGRAPHIC);

    //-----------------------TRIANGLE-------------------------
    //Instantiate an entity with the MeshRenderer component attached
    auto *mesh_renderer = Application::scene->instantiate<MeshRenderer>();
    //Attach custom component
    mesh_renderer->entity->attach<Rotator>();
    //Assign Mesh
    auto mesh = new Mesh();
    std::vector<vec3> vertex_positions = {
            vec3(-200, -100, 0.),
            vec3(200, -100, 0.),
            vec3(0., 200, 0.),
    };
    mesh->setBuffer(0, vertex_positions);
    mesh_renderer->setMesh(*mesh);
    //Assign material
    auto material = new Material();
    material->setPipeline(Graphics::DEFAULT_MESH_PIPELINE);
    material->setColor(vec4(1, 0, 0, 1));
    mesh_renderer->setMaterial(*material);

    //-----------------------EVENTS------------------
    Application::onUpdate.subscribe(&onUpdate);
    //-----------------------LOOP--------------------
    Application::run();
    //-----------------------CLEANUP------------------
    Application::onUpdate.unsubscribe(&onUpdate);
    delete mesh;
    delete material;

    //-----------------------TERMINATE------------------
    Application::terminate();
}