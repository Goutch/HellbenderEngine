//
// Created by User on 03-Jun.-2020.
//

#include <core/graphics/Graphics.h>
#include "core/entity/Entity.h"
#include "MeshRenderer.h"

void MeshRenderer::onAttach() {
    Component::onAttach();
    shader=Graphics::DEFAULT_MESH_SHADER;
}

void MeshRenderer::onDraw() {
    Graphics::draw(*entity, *mesh, *shader);
}

void MeshRenderer::setMesh(const Mesh &mesh){
    this->mesh=&mesh;
}

void MeshRenderer::setShader(const ShaderProgram &shader) {
    this->shader=&shader;
}


