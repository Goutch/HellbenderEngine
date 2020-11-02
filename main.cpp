#include "HBE.h"

int main() {
    HBE::init();

    Camera* camera=HBE::current_scene->instantiate<Camera>();
    camera->setRenderMode(ORTHOGRAPHIC);
    Entity *triangle_entity = HBE::current_scene->instantiate();
    MeshRenderer *mesh_renderer = triangle_entity->attach<MeshRenderer>();

    Mesh *mesh = Resource::create<Mesh>();

    std::vector<vec3> vertex_positions = {
            vec3(-0.25, -.25, 0.),
            vec3(0.25, -0.25, 0.),
            vec3(0., 0.25, 0.),
    };
    mesh->setBuffer(0, vertex_positions);
    mesh_renderer->setMesh(*mesh);

    //Create material
    Material *material = Resource::create<Material>();
    material->setShader(Graphics::DEFAULT_MESH_SHADER);
    //Set the color to red
    material->setColor(vec4(1,0,0,1));

    //Set the material of the mesh renderer
    mesh_renderer->setMaterial(*material);
    HBE::run();

    //-----------------------CLEANUP------------------
    //Delete ressources created with Resource.create<>()
    delete mesh;
    delete material;

    //terminate engine
    HBE::terminate();
}