#pragma once

#include "Core.h"
#include "string"
#include "vector"
#include "glm/glm.hpp"
#include "core/graphics/DrawFlags.h"

namespace HBE {
	class Shader;

	class Texture;

	class VertexLayout;

	class HB_API GraphicPipeline {
	public:
		virtual ~GraphicPipeline() = default;

		virtual void bind() const = 0;

		virtual void unbind() const = 0;

		virtual void setDrawFlags(DRAW_FLAGS flags) = 0;

		virtual DRAW_FLAGS getDrawFlags() const = 0;

		virtual void setShaders(const Shader *vertex, const Shader *fragment, const VertexLayout *layout) = 0;

		virtual void setShaders(const Shader *vertex, const Shader *geometry, const Shader *fragment,
								const VertexLayout *layout) = 0;


		virtual void setDynamicUniform(const std::string &name, void *data) = 0;
		virtual void setDynamicUniform(uint32_t binding, void *data) = 0;

		virtual void setUniform(const std::string &name, void *data) = 0;
		virtual void setUniform(uint32_t binding, void *data) = 0;
		virtual void PushConstant(const std::string &name, void *data) = 0;
		virtual void setTexture(uint32_t binding, const Texture *texture) = 0;
		virtual void setTexture(const std::string &name, const Texture *texture) = 0;
	};


}
