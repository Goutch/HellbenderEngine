
#include "InstancesRenderer.h"
#include "core/serialization/Serializer.h"

void InstancesRenderer::onAttach() {
    Component::onAttach();
    subscribeDraw();
}

void InstancesRenderer::onDraw() {
    Component::onDraw();
    Graphics::drawInstanced(*mesh, *material);
}

void InstancesRenderer::setMaterial(const Material &material) {
    this->material = &material;
}

void InstancesRenderer::setMesh(Mesh &mesh) {
    this->mesh = &mesh;
}

void InstancesRenderer::serialize(Serializer *serializer) const {
    serializer->begin(toString());
    serializer->end();
}

void InstancesRenderer::deserialize(Deserializer *deserializer) {

}


std::string InstancesRenderer::toString() const {
    return "InstancesRenderer";
}




