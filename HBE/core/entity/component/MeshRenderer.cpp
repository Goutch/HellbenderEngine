//
// Created by User on 03-Jun.-2020.
//

#include <core/graphics/Graphics.h>
#include "core/entity/Entity.h"
#include "MeshRenderer.h"

#include "core/serialization/Serializer.h"

namespace HBE {
    void MeshRenderer::onAttach() {
        Component::onAttach();
        subscribeDraw();
    }

    void MeshRenderer::onDraw() {
        if (mesh&&material)
            Graphics::draw(*entity->transform, *mesh, *material);
        else
            Log::warning(entity->getName()+" does not have a material and/or a mesh assigned");
    }

    void MeshRenderer::setMesh(const Mesh &mesh) {
        this->mesh = &mesh;
    }

    const Mesh *MeshRenderer::getMesh() {
        return mesh;
    }

    void MeshRenderer::setMaterial(const Material &material) {
        this->material = &material;
    }

    void MeshRenderer::serialize(Serializer *serializer) const {
        serializer->begin(toString());
        //todo:serialize material and mesh
        serializer->end();
    }

    std::string MeshRenderer::toString() const {
        return "MeshRenderer";
    }
}

