#pragma once

#include "Resource.h"
#include "string"
#include "Core.h"
namespace HBE {
	class Texture;

	class RenderTarget;

	class HB_API IPipelineInstance : public Resource {
	public:
		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual void setDynamicUniform(uint32_t binding, const void *data) = 0;
		virtual void setUniform(uint32_t binding, const void *data) = 0;
		virtual void setTexture(uint32_t binding, const Texture *texture, uint32_t mip_level = 0) = 0;
		virtual void setTexture(uint32_t binding, const RenderTarget *render_target) = 0;

		virtual void setDynamicUniform(const std::string &name, const void *data) = 0;
		virtual void setUniform(const std::string &name, const void *data) = 0;
		virtual void setTexture(const std::string &name, const Texture *texture, uint32_t mip_level = 0) = 0;
		virtual void setTexture(const std::string &name, const RenderTarget *render_target) = 0;
	};
}
