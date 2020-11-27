
#include "Material.h"
#include "core/utility/Log.h"
#include "core/serialization/Serializer.h"
Material::Material() {
    shader = Graphics::DEFAULT_MESH_SHADER;
}

void Material::bind() const {
    if(shader!= nullptr)
    {
        shader->bind();
        for (auto p:properties) {
            switch (p.second.type) {
                case GLSL_TYPE::FLOAT:
                    shader->setUniform(p.first, *static_cast<float*>(p.second.value));
                    break;
                case GLSL_TYPE::VEC2:
                    shader->setUniform(p.first, *static_cast<vec2*>(p.second.value));
                    break;
                case GLSL_TYPE::VEC3:
                    shader->setUniform(p.first, *static_cast<vec3*>(p.second.value));
                    break;
                case GLSL_TYPE::VEC4:
                    shader->setUniform(p.first, *static_cast<vec4*>(p.second.value));
                    break;
                case GLSL_TYPE::MAT4:
                    shader->setUniform(p.first, *static_cast<mat4*>(p.second.value));
                    break;
                case GLSL_TYPE::INT:
                    shader->setUniform(p.first, *static_cast<int*>(p.second.value));
                    break;
            }
        }
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

void Material::setProperty(std::string name, mat4 &value) {
    auto it=properties.find(name);
    if(it==properties.end())
    {
        properties.emplace(name,MaterialProperty(value));
    }
    else
    {
        it->second=MaterialProperty(value);
    }

}
void Material::setProperty(std::string name, vec4 &value) {
    auto it=properties.find(name);
    if(it==properties.end())
    {
        properties.emplace(name,MaterialProperty(value));
    }
    else
    {
        it->second=MaterialProperty(value);
    }
}
void Material::setProperty(std::string name, vec3 &value) {
    auto it=properties.find(name);
    if(it==properties.end())
    {
        properties.emplace(name,MaterialProperty(value));
    }
    else
    {
        it->second=MaterialProperty(value);
    }
}
void Material::setProperty(std::string name, vec2 &value) {
    auto it=properties.find(name);
    if(it==properties.end())
    {
        properties.emplace(name,MaterialProperty(value));
    }
    else
    {
        it->second=MaterialProperty(value);
    }
}
void Material::setProperty(std::string name, float value) {
    auto it=properties.find(name);
    if(it==properties.end())
    {
        properties.emplace(name,MaterialProperty(value));
    }
    else
    {
        it->second=MaterialProperty(value);
    }
}
void Material::setProperty(std::string name, int value) {
    auto it=properties.find(name);
    if(it==properties.end())
    {
        properties.emplace(name,MaterialProperty(value));
    }
    else
    {
        it->second=MaterialProperty(value);
    }
}

Material *Material::create() {
    return new Material();
}

void Material::serialize(Serializer *serializer) const {
    serializer->begin("Material");
    if(shader)
        shader->serialize(serializer);
    serializer->end();
}

void Material::deserialize(Deserializer *deserializer) {

}


