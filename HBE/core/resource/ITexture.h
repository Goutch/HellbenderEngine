#pragma once

#include "Texture.h"
namespace HBE{

    class ITexture
    {
    public:
        virtual ~ITexture() = default;
        virtual void setData(unsigned char *data, int width, int height, TEXTURE_FORMAT texture_type) = 0;

        virtual void bind(unsigned int slot = 0) const = 0;

        virtual void unbind(unsigned int slot = 0) const = 0;
    };
}