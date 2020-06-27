#pragma once

#include <core/resource/ShaderProgram.h>
#include <core/resource/Mesh.h>
#include <core/entity/Transform.h>
#include "Component.h"
class InstancesRenderer: public Component {

    Mesh* mesh;
    const ShaderProgram* shader;
    void onAttach() override;

    void onDraw() override;

public:
    void setShader(const ShaderProgram &shader);

    void setMesh(Mesh &mesh);
};

