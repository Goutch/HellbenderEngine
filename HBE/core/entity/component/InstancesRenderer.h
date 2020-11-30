#pragma once

#include <core/resource/Material.h>
#include <core/resource/Mesh.h>
#include <core/entity/component/Transform.h>
#include "Component.h"
#include "Core.h"
namespace HBE {
    class HB_API InstancesRenderer : public Component {

        Mesh *mesh = nullptr;
        const Material *material = nullptr;

        void onAttach() override;

        void onDraw() override;

    public:
        void setMaterial(const Material &material);

        void setMesh(Mesh &mesh);

        std::string toString() const override;

        void serialize(Serializer *serializer) const override;

        void deserialize(Deserializer *deserializer);

    };
}
