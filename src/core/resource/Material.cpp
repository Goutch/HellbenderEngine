//
// Created by User on 2020-06-27.
//

#include "Material.h"

Material::Material() {
    shader = Graphics::DEFAULT_MESH_SHADER;
}

void Material::bind() const {
    shader->bind();
    if (texture) {
        texture->bind(0);
        shader->setUniform("has_texture", true);
        shader->setUniform("texture", 0);
    }
    if (normal_map) {
        normal_map->bind(1);
        shader->setUniform("has_normal_map", true);
        shader->setUniform("normal_map", 1);
    }
    if (specular_map) {
        specular_map->bind(2);
        shader->setUniform("specular_map", 2);
        shader->setUniform("has_specular_map", true);
    }
    shader->setUniform("material_color", color);
}

void Material::unbind() const {
    if (texture)texture->unbind(0);
    if (normal_map)normal_map->unbind(1);
    if (specular_map)specular_map->unbind(1);
    shader->unbind();
}

void Material::setShader(const ShaderProgram *shader) {
    this->shader = shader;
}

void Material::setTexture(const Texture *texture) {
    this->texture = texture;
}

void Material::setSpecularMap(const Texture *texture) {
    this->specular_map = texture;
}

void Material::setNormalMap(const Texture *texture) {
    this->normal_map = texture;
}

const ShaderProgram &Material::getShader() const {
    return *shader;
}

