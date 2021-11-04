#pragma once

#include <core/resource/Resource.h>
#include "Core.h"
#include "core/utility/Event.h"
#include "Texture.h"

namespace HBE {
	class Framebuffer;

	class GraphicPipeline;

	enum RENDER_TARGET_FLAGS {
		RENDER_TARGET_FLAG_NONE=0,
		RENDER_TARGET_FLAG_DONT_CLEAR=1
	};
	struct RenderTargetInfo {
		uint32_t width = 0;
		uint32_t height = 0;
		IMAGE_FORMAT format = IMAGE_FORMAT_RGBA8;
		vec4 clear_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		RENDER_TARGET_FLAGS flags = RENDER_TARGET_FLAG_NONE;
	};

	class HB_API RenderTarget : public Resource {

	public:
		mutable Event<RenderTarget *> onResolutionChange;
		virtual void setResolution(uint32_t width, uint32_t height) = 0;
		virtual void getResolution(uint32_t &width, uint32_t &height) const = 0;
		virtual void setClearColor(vec4 color) = 0;
		virtual const vec4 &getClearColor() const = 0;

	};
}
