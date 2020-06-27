
#include "InstancesRenderer.h"
#include "core/graphics/Graphics.h"
void InstancesRenderer::onAttach() {
    Component::onAttach();
    subscribeDraw();
}

void InstancesRenderer::onDraw() {
    Component::onDraw();
    Graphics::drawInstanced(*mesh,*shader);
}

void InstancesRenderer::setShader(const ShaderProgram &shader) {
    this->shader=&shader;
}

void InstancesRenderer::setMesh(Mesh &mesh) {
    this->mesh=&mesh;
}
