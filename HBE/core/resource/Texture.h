#pragma once

#include "Core.h"
#include <string>
#include "Resource.h"

namespace HBE {

    enum IMAGE_FORMAT {
        IMAGE_FORMAT_R8 = 0,
        IMAGE_FORMAT_RG8 = 1,
        IMAGE_FORMAT_RGB8 = 2,
        IMAGE_FORMAT_RGBA8 = 3,
        IMAGE_FORMAT_R32F = 4,
        IMAGE_FORMAT_RG32F = 5,
        IMAGE_FORMAT_RGB32F = 6,
        IMAGE_FORMAT_RGBA32F = 7,
        IMAGE_FORMAT_DEPTH32F = 8,
        IMAGE_FORMAT_DEPTH32f_STENCIL8U = 9,
        IMAGE_FORMAT_DEPTH24f_STENCIL8U = 10,
    };
    typedef uint32_t IMAGE_FLAGS;
    enum IMAGE_FLAG {
        IMAGE_FLAG_NONE = 0,
        IMAGE_FLAG_SHADER_WRITE = 1 << 0,
        IMAGE_FLAG_RENDER_TARGET = 1 << 1,
        IMAGE_FLAG_DEPTH = 1 << 2,
        IMAGE_FLAG_NO_SAMPLER = 1 << 3,// image2D in glsl aecessed with imageLoad and imageStore.
    };
    /**
     * @param data can be nullptr and updated later
     * @param width must be greater than 0
     * @param height must be greater than 0
     * @param depth must be greater than 0
     */
    struct TextureInfo {
        void *data = nullptr;
        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t depth = 1;
        IMAGE_FORMAT format = IMAGE_FORMAT_RGBA8;
        IMAGE_FLAGS flags = IMAGE_FLAG_NONE;

    };

    class HB_API Texture : public Resource {

    public:
        virtual ~Texture() = default;

        virtual uint32_t getWidth() const = 0;

        virtual uint32_t getHeight() const = 0;

        virtual uint32_t getDepth() const = 0;

        virtual vec3u getSize() const = 0;

        virtual void update(const void *data) = 0;

        static Texture *load(std::string path);

    };

}