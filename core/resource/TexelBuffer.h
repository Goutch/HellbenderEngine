#pragma once

#include "Core.h"
#include "Image.h"

namespace HBE {
    typedef uint32_t TEXEL_BUFFER_FLAGS;
    enum TEXEL_BUFFER_FLAG {
        TEXEL_BUFFER_FLAG_NONE = 0,
        TEXEL_BUFFER_FLAG_MAPPABLE = 1, //Can be read from CPU
    };
    struct TexelBufferInfo {
        IMAGE_FORMAT format = IMAGE_FORMAT_R8;
        uint32_t count = 0;
        TEXEL_BUFFER_FLAGS flags = TEXEL_BUFFER_FLAG_NONE;
    };

    class HB_API TexelBuffer : public Resource {
    public:

        virtual void update(const void *data) = 0;

        virtual ~TexelBuffer() {};
    };
}
