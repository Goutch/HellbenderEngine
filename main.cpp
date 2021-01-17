#include "HBE.h"

using namespace HBE;

class Rotator : public Component {
public:
    void onAttach() override
    {
        //note:there is no need to unsubscribe in the onDetach function
        subscribeUpdate();
    }
    void onUpdate(float delta) override {
        entity->transform->rotate(delta, vec3(0, 1, 0));
    }
};

int main() {
    Application::init();
    Graphics::setDefaultDrawFlags(DRAW_FLAGS_NONE);
    //-----------------------CAMERA--------------------------
    //Instanciate an entity with the camera component attached
    Camera *camera = Application::scene->instantiate<Camera>();

    //Set rendering mode to 2 dimensions
    camera->setRenderMode(ORTHOGRAPHIC);

    //-----------------------TRIANGLE ENTITY------------------
    //Create triangle entity
    Entity *triangle_entity = Application::scene->instantiate();

    //Attach the custom Component to the entity
    triangle_entity->attach<Rotator>();

    //Attach mesh renderer component
    MeshRenderer *mesh_renderer = triangle_entity->attach<MeshRenderer>();

    //Create the resources the mesh renderer need
    Mesh *mesh = Mesh::create();
    Material *material = Material::create();

    //Assign the resources to the mesh renderer
    mesh_renderer->setMaterial(*material);
    mesh_renderer->setMesh(*mesh);

    //Init Mesh
    std::vector<vec3> vertex_positions = {
            vec3(-200, -100, 0.),
            vec3(200, -100, 0.),
            vec3(0., 200, 0.),
    };
    mesh->setBuffer(0, vertex_positions);


    //Init material
    material->setShader(Graphics::DEFAULT_MESH_SHADER);
    material->setColor(vec4(1, 0, 0, 1));

    Application::run();

    //-----------------------CLEANUP------------------
    //Delete ressources created
    delete mesh;
    delete material;

    //terminate engine
    Application::terminate();
}