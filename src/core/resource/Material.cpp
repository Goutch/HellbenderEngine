//
// Created by User on 2020-06-27.
//

#include "Material.h"

Material::Material() {
    shader = Graphics::DEFAULT_MESH_SHADER;
}

void Material::bind() const {
    if(shader!= nullptr)
    {
        shader->bind();
        shader->setUniform("material_color", color);
        if (texture) {
            shader->setUniform("has_texture", true);
            shader->setUniform("texture_0", 0);
            texture->bind(0);
        } else {
            shader->setUniform("has_texture", false);
        }
        if (normal_map) {
            shader->setUniform("has_normal_map", true);
            shader->setUniform("normal_map", 1);
            normal_map->bind(1);
        } else {
            shader->setUniform("has_normal_map", false);
        }
        if (specular_map) {
            shader->setUniform("specular_map", 2);
            shader->setUniform("has_specular_map", true);
            specular_map->bind(2);
        } else {
            shader->setUniform("has_specular_map", false);
        }
    }

}

void Material::unbind() const {
    if (texture)texture->unbind(0);
    if (normal_map)normal_map->unbind(1);
    if (specular_map)specular_map->unbind(2);
    shader->unbind();
}

void Material::setShader(const ShaderProgram *shader) {
    this->shader = shader;
}

void Material::setTexture(const Texture *texture) {
    this->texture = texture;
}

void Material::setNormalMap(const Texture *normal_map) {
    this->normal_map = normal_map;
}

void Material::setSpecularMap(const Texture *specular_map) {
    this->specular_map = specular_map;
}

const ShaderProgram &Material::getShader() const {
    return *shader;
}

void Material::setColor(vec4 color) {
    this->color = color;
}

