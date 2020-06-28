#pragma once

#include "Texture.h"
#include "ShaderProgram.h"
#include "core/graphics/Graphics.h"
class Material {
    const ShaderProgram *shader= nullptr;
    const Texture *texture = nullptr;
    const Texture *normal_map = nullptr;
    const Texture *specular_map = nullptr;
    glm::vec4 color=glm::vec4(1,1,1,1);
    bool is_transparent = false;
public:
    Material();
    void bind() const;
    void unbind() const;
    void setShader(const ShaderProgram* shaderProgram);
    void setTexture(const Texture *texture);
    void setSpecularMap(const Texture* texture);
    void setNormalMap(const Texture* texture);
    const ShaderProgram& getShader() const;
};
