#pragma once

#include "Core.h"
#include <core/entity/component/Component.h>

namespace HBE {
    class Material;
    class Mesh;

    class HB_API MeshRenderer : public Component {
        const Mesh *mesh;
        const Material *material;

    public:
        void onAttach() override;

        void onDraw() override;

        void setMaterial(const Material &material);

        void setMesh(const Mesh &mesh);

        const Mesh *getMesh();

        std::string toString() const override;

        void serialize(Serializer *serializer) const override;
    };
}