#pragma once


#include <core/entity/component/Component.h>
class Material;
class MeshRenderer : public Component {
    const Mesh *mesh;
    const Material *material;

public:
    void onAttach() override;

    void onDraw() override;

    void setMaterial(const Material &material);

    void setMesh(const Mesh &mesh);


};


