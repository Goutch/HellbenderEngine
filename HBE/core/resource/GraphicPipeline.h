#pragma once

#include "core/serialization/Serializable.h"

#include "glm/glm.hpp"
#include "string"
#include "vector"
#include "Core.h"
#include "map"

#include "Resource.h"
#include "core/graphics/DrawFlags.h"
namespace HBE {

    class Shader;

    class IGraphicPipeline;

    class HB_API GraphicPipeline final : public Resource {
        IGraphicPipeline *instance;
    public:
        GraphicPipeline();

        ~GraphicPipeline();

        void setShaders(const Shader *vertex, const Shader *geometry,const Shader *fragment);

        void setShaders(const Shader *vertex, const Shader *fragment);

        void bind() const;

        void unbind() const;

        void setDrawFlags(DRAW_FLAGS flags);

        DRAW_FLAGS getDrawFlags() const;

        void setUniform(const std::string &name, int i) const;

        void setUniform(const std::string &name, float f) const;

        void setUniform(const std::string &name, const glm::vec2 &v) const;

        void setUniform(const std::string &name, const glm::vec3 &v) const;

        void setUniform(const std::string &name, const glm::vec4 &v) const;

        void setUniform(const std::string &name, const glm::mat3 &m) const;

        void setUniform(const std::string &name, const glm::mat4 &m) const;

        void setUniform(unsigned int location, int i) const;

        void setUniform(unsigned int location, float f) const;

        void setUniform(unsigned int location, const glm::vec2 &v) const;

        void setUniform(unsigned int location, const glm::vec3 &v) const;

        void setUniform(unsigned int location, const glm::vec4 &v) const;

        void setUniform(unsigned int location, const glm::mat3 &m) const;

        void setUniform(unsigned int location, const glm::mat4 &m) const;

        void setUniformIntArray(unsigned int location, int *i, unsigned int count) const;

        void setUniformFloatArray(unsigned int location, float *f, unsigned int count) const;

        void setUniformVec2Array(unsigned int location, const glm::vec2 *v, unsigned int count) const;

        void setUniformVec3Array(unsigned int location, const glm::vec3 *v, unsigned int count) const;

        void setUniformVec4Array(unsigned int location, const glm::vec4 *v, unsigned int count) const;
    };
}