
#include "GraphicPipeline.h"
#include "core/utility/Log.h"

#include "IGraphicPipeline.h"
#include "core/graphics/Graphics.h"
#include "core/graphics/Renderer.h"
#include "core/resource/IResourceFactory.h"

using namespace glm;
namespace HBE {
    GraphicPipeline::~GraphicPipeline() {
        delete instance;
    }

    GraphicPipeline::GraphicPipeline() {
        instance = Graphics::getRenderer()->getResourceFactory()->createGraphicsPipeline();
    }

    void GraphicPipeline::bind() const {
        instance->bind();
    }

    void GraphicPipeline::unbind() const {
        instance->unbind();
    }

    void GraphicPipeline::setDrawFlags(DRAW_FLAGS flags) {
        instance->setDrawFlags(flags);
    }

    DRAW_FLAGS GraphicPipeline::getDrawFlags() const {
        return instance->getDrawFlags();
    }

    void GraphicPipeline::setUniform(const std::string &name, int i) const {
        instance->setUniform(name, i);
    }

    void GraphicPipeline::setUniform(const std::string &name, float f) const {
        instance->setUniform(name, f);
    }

    void GraphicPipeline::setUniform(const std::string &name, const vec2 &v) const {
        instance->setUniform(name, v);
    }

    void GraphicPipeline::setUniform(const std::string &name, const vec3 &v) const {
        instance->setUniform(name, v);
    }

    void GraphicPipeline::setUniform(const std::string &name, const vec4 &v) const {
        instance->setUniform(name, v);
    }

    void GraphicPipeline::setUniform(const std::string &name, const mat3 &m) const {
        instance->setUniform(name, m);
    }

    void GraphicPipeline::setUniform(const std::string &name, const mat4 &m) const {
        instance->setUniform(name, m);
    }

    void GraphicPipeline::setUniform(unsigned int location, int i) const {
        instance->setUniform(location, i);
    }

    void GraphicPipeline::setUniform(unsigned int location, float f) const {
        instance->setUniform(location, f);
    }

    void GraphicPipeline::setUniform(unsigned int location, const vec2 &v) const {
        instance->setUniform(location, v);
    }

    void GraphicPipeline::setUniform(unsigned int location, const vec3 &v) const {
        instance->setUniform(location, v);
    }

    void GraphicPipeline::setUniform(unsigned int location, const vec4 &v) const {
        instance->setUniform(location, v);
    }

    void GraphicPipeline::setUniform(unsigned int location, const mat3 &m) const {
        instance->setUniform(location, m);
    }

    void GraphicPipeline::setUniform(unsigned int location, const mat4 &m) const {
        instance->setUniform(location, m);
    }

    void GraphicPipeline::setUniformIntArray(unsigned int location, int *i, unsigned int count) const {
        instance->setUniformIntArray(location, i, count);
    }

    void GraphicPipeline::setUniformFloatArray(unsigned int location, float *f, unsigned int count) const {
        instance->setUniformFloatArray(location, f, count);
    }

    void GraphicPipeline::setUniformVec2Array(unsigned int location, const glm::vec2 *v, unsigned int count) const {
        instance->setUniformVec2Array(location, v, count);
    }

    void GraphicPipeline::setUniformVec3Array(unsigned int location, const glm::vec3 *v, unsigned int count) const {
        instance->setUniformVec3Array(location, v, count);
    }

    void GraphicPipeline::setUniformVec4Array(unsigned int location, const glm::vec4 *v, unsigned int count) const {
        instance->setUniformVec4Array(location, v, count);
    }

    void GraphicPipeline::setShaders(const Shader *vertex, const Shader *fragment) {
        instance->setShaders(vertex, fragment);
    }

    void GraphicPipeline::setShaders(const Shader *vertex, const Shader *geometry, const Shader *fragment) {
        instance->setShaders(vertex, geometry, fragment);
    }


}