#pragma once

#include <core/resource/Texture.h>
namespace HBE {
    class GL_Texture : public Texture {

    private:
        unsigned int texture_id;
    public:
        GL_Texture();

        ~GL_Texture();
		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;
		virtual uint32_t getDepth() const = 0;
		virtual void setData(void *data, uint32_t width = 1, uint32_t height = 1, uint32_t depth = 1, IMAGE_FORMAT format = IMAGE_FORMAT_RGBA8, IMAGE_FLAGS flags = IMAGE_FLAG_NONE) = 0;


    };
}