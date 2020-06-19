#include <Core.h>
#include <graphics/gl/GL_Mesh.h>
#include "graphics/gl/GL_ShaderProgram.h"
#define NUMBER_CUBES 1000
#define RANGE 100
int main() {
    PrototypeEngine::init();
    Scene* current_scene=PrototypeEngine::setScene("");

    Mesh* cube=new GL_Mesh();
    Geometry::createCube(*cube,1,1,1,{true,true,false});

    auto camera_entity=current_scene->instantiate("camera");
    auto camera=camera_entity->attach<Camera>();
    auto camera_controller=camera_entity->attach<CameraController>();
    camera->setAspectRatio(WIDTH,HEIGHT);

    auto mesh_entities=std::array<Entity*,NUMBER_CUBES>();
    auto mesh_renderers=std::array<MeshRenderer*,NUMBER_CUBES>();

    for (int i = 0; i < NUMBER_CUBES; ++i) {
        mesh_entities[i]=PrototypeEngine::current_scene->instantiate();
        mesh_renderers[i]=mesh_entities[i]->attach<MeshRenderer>();
        mesh_renderers[i]->setShader(*Graphics::DEFAULT_MESH_SHADER);
        mesh_renderers[i]->setMesh(*cube);
        mesh_entities[i]->translate(vec3((rand()%(RANGE*2))-RANGE,(rand()%(RANGE*2))-RANGE,(rand()%(RANGE*2))-RANGE));
    }

    auto ground_entity=current_scene->instantiate("ground");
    auto ground_mr=ground_entity->attach<MeshRenderer>();
    ground_mr->setMesh(*Graphics::DEFAULT_QUAD);
    ground_mr->setShader(*Graphics::DEFAULT_MESH_SHADER);
    ground_entity->rotate(glm::radians(-90.0f),vec3(1,0,0));
    ground_entity->scale(vec3(5000,5000,1));
    PrototypeEngine::run();
    PrototypeEngine::terminate();
}


