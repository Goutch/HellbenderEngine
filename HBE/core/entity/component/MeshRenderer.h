#pragma once


#include <core/entity/component/Component.h>
class Material;
class Mesh;
class MeshRenderer : public Component {
    const Mesh *mesh;
    const Material *material;

public:
    void onAttach() override;

    void onDraw() override;

    void setMaterial(const Material &material);

    void setMesh(const Mesh &mesh);
    const Mesh* getMesh();

    void serialize(Serializer* serializer) const override;
};


