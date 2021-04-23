#pragma once

#include "Texture.h"
#include "GraphicPipeline.h"
#include "core/graphics/Graphics.h"
#include "unordered_map"
#include "core/serialization/Serializable.h"
#include "Core.h"


using namespace glm;
namespace HBE {
    enum GLSL_TYPE {
        FLOAT,
        VEC2,
        VEC3,
        VEC4,
        MAT4,
        INT,
    };

    struct HB_API MaterialProperty {
        GLSL_TYPE type;
        void *value;

        MaterialProperty(float v) {
            type = GLSL_TYPE::FLOAT;
            value = new float(v);
        }

        MaterialProperty(int v) {
            type = GLSL_TYPE::INT;
            value = new int(v);
        }

        MaterialProperty(vec2 v) {
            type = GLSL_TYPE::VEC2;
            value = new vec2(v);
        }

        MaterialProperty(vec3 v) {
            type = GLSL_TYPE::VEC3;
            value = new vec3(v);
        }

        MaterialProperty(vec4 v) {
            type = GLSL_TYPE::VEC4;
            value = new vec4(v);
        }

        MaterialProperty(mat4 v) {
            type = GLSL_TYPE::MAT4;
            value = new mat4(v);
        }
    };

    class HB_API Material final : Serializable, public Resource {
        friend class Resource;

        const GraphicPipeline *pipeline = nullptr;
        const Texture *texture = nullptr;
        const Texture *normal_map = nullptr;
        const Texture *specular_map = nullptr;
        glm::vec4 color = glm::vec4(1, 1, 1, 1);
        std::unordered_map<std::string, MaterialProperty> properties;
    public:
        void bind() const;

        void unbind() const;

        void setPipeline(const GraphicPipeline *pipeline);

        void setTexture(const Texture *texture);

        void setSpecularMap(const Texture *texture);

        void setNormalMap(const Texture *texture);

        const GraphicPipeline *getPipeline() const;

        void setColor(vec4 color);

        void setProperty(std::string name, mat4 &value);

        void setProperty(std::string name, int value);

        void setProperty(std::string name, float value);

        void setProperty(std::string name, vec2 &value);

        void setProperty(std::string name, vec3 &value);

        void setProperty(std::string name, vec4 &value);

        void serialize(Serializer *serializer) const override;

        Material();

    private:
        void deserialize(Deserializer *deserializer) override;
    };
}