#pragma once

#include <core/resource/Resource.h>
#include "Core.h"
#include "Image.h"
#include "Event.h"

using namespace utils;
namespace HBE {
	class Framebuffer;

	class RasterizationPipeline;

	typedef uint32_t RENDER_TARGET_FLAGS;
	enum RENDER_TARGET_FLAG {
		RENDER_TARGET_FLAG_NONE = 0,
		RENDER_TARGET_FLAG_DEPTH_ATTACHMENT = 1,
		RENDER_TARGET_FLAG_COLOR_ATTACHMENT = 2,
		RENDER_TARGET_FLAG_CLEAR_COLOR = 4,
		RENDER_TARGET_FLAG_CLEAR_DEPTH = 8,
	};


	struct RenderTargetInfo {
		uint32_t width = 0;
		uint32_t height = 0;

		IMAGE_FORMAT format = IMAGE_FORMAT_RGBA32F;
		vec4 clear_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);

		RENDER_TARGET_FLAGS flags = RENDER_TARGET_FLAG_NONE;
	};

	class HB_API RasterizationTarget : public Resource {

	public:
		mutable Event<RasterizationTarget *> onResolutionChange;

		virtual void setResolution(uint32_t width, uint32_t height) = 0;

		virtual vec2i getResolution() const = 0;

		virtual void setClearColor(vec4 color) = 0;

		virtual const vec4 &getClearColor() const = 0;

		virtual Image *getFramebufferTexture(uint32_t frame) const = 0;
	};
}
