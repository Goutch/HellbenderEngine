#pragma once

#include <string>
#include "core/serialization/Serializable.h"
#include "Core.h"
#include "Resource.h"

namespace HBE {
    enum TEXTURE_FORMAT {
        R8,
        RG8,
        RGB8,
        RGBA8,
        R32F,
        RG32F,
        RGB32F,
        RGBA32F,
    };

    class HB_API Texture : public Resource {
        std::string path;
        int width = 1;
        int height = 1;
    public:
        virtual ~Texture()=default;

        void load(std::string path);

        virtual void setData(unsigned char *data, int width, int height, TEXTURE_FORMAT format) = 0;

        virtual void bind(unsigned int slot) const = 0;

        virtual void unbind(unsigned int slot) const = 0;
    };

}