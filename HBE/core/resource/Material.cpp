
#include "Material.h"
#include "core/utility/Log.h"
#include "core/serialization/Serializer.h"
namespace HBE {
    Material::Material() {
        pipeline = Graphics::DEFAULT_MESH_PIPELINE;
    }

    void Material::bind() const {
        if (pipeline != nullptr) {
            pipeline->bind();
            for (auto p:properties) {
                switch (p.second.type) {
                    //todo:
                    case GLSL_TYPE::FLOAT:
                        pipeline->setUniform(p.first, *static_cast<float *>(p.second.value));
                        break;
                    case GLSL_TYPE::VEC2F:
                        pipeline->setUniform(p.first, *static_cast<vec2 *>(p.second.value));
                        break;
                    case GLSL_TYPE::VEC3F:
                        pipeline->setUniform(p.first, *static_cast<vec3 *>(p.second.value));
                        break;
                    case GLSL_TYPE::VEC4F:
                        pipeline->setUniform(p.first, *static_cast<vec4 *>(p.second.value));
                        break;
                    case GLSL_TYPE::MAT4F:
                        pipeline->setUniform(p.first, *static_cast<mat4 *>(p.second.value));
                        break;
                    case GLSL_TYPE::INT:
                        pipeline->setUniform(p.first, *static_cast<int *>(p.second.value));
                        break;
                    case UINT:
                        break;
                    case VEC2I:
                        break;
                    case VEC3I:
                        break;
                    case VEC4I:
                        break;
                    case VEC2UI:
                        break;
                    case VEC3UI:
                        break;
                    case VEC4UI:
                        break;
                    case MAT3F:
                        break;
                    case MAT3I:
                        break;
                    case MAT4I:
                        break;
                    case MAT3UI:
                        break;
                    case MAT4UI:
                        break;
                }
            }
            pipeline->setUniform("material_color", color);
            if (texture) {
                pipeline->setUniform("has_texture", true);
                pipeline->setUniform("texture_0", 0);
                texture->bind(0);
            } else {
                pipeline->setUniform("has_texture", false);
            }

            if (normal_map) {
                pipeline->setUniform("has_normal_map", true);
                pipeline->setUniform("normal_map", 1);
                normal_map->bind(1);
            } else {
                pipeline->setUniform("has_normal_map", false);
            }

            if (specular_map) {
                pipeline->setUniform("specular_map", 2);
                pipeline->setUniform("has_specular_map", true);
                specular_map->bind(2);
            } else {
                pipeline->setUniform("has_specular_map", false);
            }
        }

    }

    void Material::unbind() const {
        if (texture)texture->unbind(0);
        if (normal_map)normal_map->unbind(1);
        if (specular_map)specular_map->unbind(2);
        pipeline->unbind();
    }

    void Material::setPipeline(const GraphicPipeline *pipeline) {
        this->pipeline = pipeline;
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

    const GraphicPipeline *Material::getPipeline() const {
        return pipeline;
    }

    void Material::setColor(vec4 color) {
        this->color = color;
    }

    void Material::setProperty(std::string name, mat4 &value) {
        auto it = properties.find(name);
        if (it == properties.end()) {
            properties.emplace(name, MaterialProperty(value));
        } else {
            it->second = MaterialProperty(value);
        }

    }

    void Material::setProperty(std::string name, vec4 &value) {
        auto it = properties.find(name);
        if (it == properties.end()) {
            properties.emplace(name, MaterialProperty(value));
        } else {
            it->second = MaterialProperty(value);
        }
    }

    void Material::setProperty(std::string name, vec3 &value) {
        auto it = properties.find(name);
        if (it == properties.end()) {
            properties.emplace(name, MaterialProperty(value));
        } else {
            it->second = MaterialProperty(value);
        }
    }

    void Material::setProperty(std::string name, vec2 &value) {
        auto it = properties.find(name);
        if (it == properties.end()) {
            properties.emplace(name, MaterialProperty(value));
        } else {
            it->second = MaterialProperty(value);
        }
    }

    void Material::setProperty(std::string name, float value) {
        auto it = properties.find(name);
        if (it == properties.end()) {
            properties.emplace(name, MaterialProperty(value));
        } else {
            it->second = MaterialProperty(value);
        }
    }

    void Material::setProperty(std::string name, int value) {
        auto it = properties.find(name);
        if (it == properties.end()) {
            properties.emplace(name, MaterialProperty(value));
        } else {
            it->second = MaterialProperty(value);
        }
    }
}

