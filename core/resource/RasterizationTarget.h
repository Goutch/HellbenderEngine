#pragma once

#include <core/resource/Resource.h>
#include "Core.h"
#include "Image.h"
#include "dependencies/utils-collection/Event.h"

namespace HBE {
    class Framebuffer;

    class RasterizationPipeline;

    typedef uint8_t RENDER_TARGET_FLAGS;

    enum RENDER_TARGET_FLAG : uint8_t {
        RENDER_TARGET_FLAG_NONE = 0,
        RENDER_TARGET_FLAG_DEPTH_ATTACHMENT = 1,
        RENDER_TARGET_FLAG_COLOR_ATTACHMENT = 2,
        RENDER_TARGET_FLAG_CLEAR_COLOR = 4,
        RENDER_TARGET_FLAG_CLEAR_DEPTH = 8,
    };


    struct RasterizationTargetInfo {
        uint32_t width = 0;
        uint32_t height = 0;
        vec4 clear_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        IMAGE_FORMAT format = IMAGE_FORMAT_RGBA32F;
        RENDER_TARGET_FLAGS flags = RENDER_TARGET_FLAG_NONE;
    };

    class HB_API RasterizationTarget {
    public:
        mutable Event<RasterizationTarget *> onResolutionChange;

        void setResolution(uint32_t width, uint32_t height);

        ImageHandle getFramebufferTexture(uint32_t frame_index);
    };
}
