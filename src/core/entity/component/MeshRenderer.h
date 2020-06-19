#pragma once


#include <core/entity/component/Component.h>
class ShaderProgram;
class MeshRenderer : public Component {
    const Mesh *mesh;
    const ShaderProgram *shader;

public:
    void onAttach() override;

    void onDraw() override;

    void setShader(const ShaderProgram &shader);

    void setMesh(const Mesh &mesh);


};


