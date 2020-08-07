
#include "InstancesRenderer.h"
#include "core/graphics/Graphics.h"
void InstancesRenderer::onAttach() {
    Component::onAttach();
    subscribeDraw();
}

void InstancesRenderer::onDraw() {
    Component::onDraw();
    Graphics::drawInstanced(*mesh,*material);
}

void InstancesRenderer::setMaterial(const Material &material) {
    this->material=&material;
}

void InstancesRenderer::setMesh(Mesh &mesh) {
    this->mesh=&mesh;
}
