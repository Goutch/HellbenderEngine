#pragma once

#include <core/resource/Material.h>
#include <core/resource/Mesh.h>
#include <core/entity/Transform.h>
#include "Component.h"

class InstancesRenderer : public Component {

    Mesh *mesh;
    const Material *material;

    void onAttach() override;

    void onDraw() override;

public:
    void setMaterial(const Material &material);

    void setMesh(Mesh &mesh);

    void serialize(Serializer* serializer) const override;
};

