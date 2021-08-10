#pragma once

#include "string"
#include "vector"
#include "glm/glm.hpp"
#include "core/graphics/DrawFlags.h"

namespace HBE {
	class Shader;

	class VertexLayout;

	class GraphicPipeline {
	public:
		virtual ~GraphicPipeline() = default;

		virtual void bind() const = 0;

		virtual void unbind() const = 0;

		virtual void setDrawFlags(DRAW_FLAGS flags) = 0;

		virtual DRAW_FLAGS getDrawFlags() const = 0;

		virtual void setShaders(const Shader *vertex, const Shader *fragment, const VertexLayout *layout) = 0;

		virtual void setShaders(const Shader *vertex, const Shader *geometry, const Shader *fragment,
								const VertexLayout *layout) = 0;

		virtual void setUniform(const std::string &name, void *data) const = 0;

		virtual void setUniform(const std::string &name, int i) const = 0;

		virtual void setUniform(const std::string &name, float f) const = 0;

		virtual void setUniform(const std::string &name, const glm::vec2 &v) const = 0;

		virtual void setUniform(const std::string &name, const glm::vec3 &v) const = 0;

		virtual void setUniform(const std::string &name, const glm::vec4 &v) const = 0;

		virtual void setUniform(const std::string &name, const glm::mat3 &m) const = 0;

		virtual void setUniform(const std::string &name, const glm::mat4 &m) const = 0;


		virtual void setUniform(uint32_t binding, void *data, uint32_t byte_count) const = 0;

		virtual void setUniform(unsigned int location, int i) const = 0;

		virtual void setUniform(unsigned int location, float f) const = 0;

		virtual void setUniform(unsigned int location, const glm::vec2 &v) const = 0;

		virtual void setUniform(unsigned int location, const glm::vec3 &v) const = 0;

		virtual void setUniform(unsigned int location, const glm::vec4 &v) const = 0;

		virtual void setUniform(unsigned int location, const glm::mat3 &m) const = 0;

		virtual void setUniform(unsigned int location, const glm::mat4 &m) const = 0;

		virtual void setUniformIntArray(unsigned int location, int *i, unsigned int count) const = 0;

		virtual void setUniformFloatArray(unsigned int location, float *f, unsigned int count) const = 0;

		virtual void setUniformVec2Array(unsigned int location, const glm::vec2 *v, unsigned int count) const = 0;

		virtual void setUniformVec3Array(unsigned int location, const glm::vec3 *v, unsigned int count) const = 0;

		virtual void setUniformVec4Array(unsigned int location, const glm::vec4 *v, unsigned int count) const = 0;

	};
}
