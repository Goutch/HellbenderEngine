
#include "HBE.h"

using namespace HBE;

int main() {
    Application::init();
    Graphics::setDefaultDrawFlags(DRAW_FLAGS_NONE);
    Camera *camera = Application::scene->instantiate<Camera>();
    camera->setRenderMode(ORTHOGRAPHIC);

    Entity *triangle_entity = Application::scene->instantiate();
    Mesh *mesh = new Mesh();
    Material *material = new Material();
    material->setPipeline(Graphics::DEFAULT_MESH_PIPELINE);
    material->setColor(vec4(1, 0, 0, 1));
    MeshRenderer *mesh_renderer = triangle_entity->attach<MeshRenderer>();
    mesh_renderer->setMaterial(*material);
    mesh_renderer->setMesh(*mesh);

    std::vector<vec3> vertex_positions = {
            vec3(-400, -100, 0.),
            vec3(400, -100, 0.),
            vec3(0., 400, 0.),
    };
    mesh->setBuffer(0, vertex_positions);


    Application::run();
    delete mesh;
    delete material;
    Application::terminate();
}
