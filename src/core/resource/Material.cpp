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
    }
    if (normal_map) {
        normal_map->bind(1);
    }
    if (specular_map) {
        specular_map->bind(2);
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
    setColor(color);
    setTexture(texture);
    setNormalMap(normal_map);
    setSpecularMap(specular_map);
}

void Material::setTexture(const Texture *texture) {
    if(shader!= nullptr)
    {
        shader->bind();
        if (texture != nullptr) {

            shader->setUniform("has_texture", true);
            shader->setUniform("texture_0", 0);
        } else {
            shader->setUniform("has_texture", false);
        }
        shader->unbind();
    }
    this->texture = texture;
}

void Material::setNormalMap(const Texture *normal_map) {
    if(shader!= nullptr) {
        shader->bind();
        if (normal_map) {
            shader->setUniform("has_normal_map", true);
            shader->setUniform("normal_map", 1);
        } else {
            shader->setUniform("has_normal_map", false);
        }
        shader->unbind();
    }
    this->normal_map = normal_map;
}

void Material::setSpecularMap(const Texture *specular_map) {
    if(shader!= nullptr) {
        shader->bind();
        if (specular_map) {
            shader->setUniform("specular_map", 2);
            shader->setUniform("has_specular_map", true);
        } else {
            shader->setUniform("has_specular_map", false);
        }
        shader->unbind();
    }
    this->specular_map = specular_map;
}


const ShaderProgram &Material::getShader() const {
    return *shader;
}

void Material::setColor(vec4 color) {
    this->color=color;
    shader->bind();
    shader->setUniform("material_color", color);
    shader->unbind();
}

