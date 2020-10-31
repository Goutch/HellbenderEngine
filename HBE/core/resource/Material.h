#pragma once

#include "Texture.h"
#include "ShaderProgram.h"
#include "core/graphics/Graphics.h"
#include "unordered_map"
#include "MaterialProperty.h"
class Material {
    const ShaderProgram *shader= nullptr;
    const Texture *texture = nullptr;
    const Texture *normal_map = nullptr;
    const Texture *specular_map = nullptr;
    glm::vec4 color=glm::vec4(1,1,1,1);
    bool is_transparent = false;
    std::unordered_map<std::string,MaterialProperty> properties;
public:
    Material();
    void bind() const;
    void unbind() const;
    void setShader(const ShaderProgram* shaderProgram);
    void setTexture(const Texture *texture);
    void setSpecularMap(const Texture* texture);
    void setNormalMap(const Texture* texture);
    const ShaderProgram& getShader() const;
    void setColor(vec4 color);
    void setProperty(std::string name, mat4& value);
    void setProperty(std::string name, int value);
    void setProperty(std::string name, float value);
    void setProperty(std::string name, vec2 &value);
    void setProperty(std::string name, vec3 &value);
    void setProperty(std::string name, vec4 &value);
};
