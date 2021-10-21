#pragma once

#include "Core.h"
#include "core/entity/component/Component.h"

namespace HBE {
    class Mesh;

    class HB_API MeshRenderer : public Component {
        const Mesh *mesh;
        GraphicPipeline *pipeline;

    public:
        void onAttach() override;

        void onDraw() override;

        void setMaterial( GraphicPipeline &pipeline);

        void setMesh(const Mesh &mesh);

        const Mesh *getMesh();

        std::string toString() const override;

        void serialize(Serializer *serializer) const override;
    };
}