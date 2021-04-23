#pragma once

#include <core/resource/ITexture.h>
namespace HBE {
    class GL_Texture : public ITexture {

    private:
        unsigned int texture_id;
    public:
        GL_Texture();

        ~GL_Texture();

        void setData(unsigned char *data, int width, int height, TEXTURE_FORMAT texture_type) override;

        void bind(unsigned int slot = 0) const override;

        void unbind(unsigned int slot = 0) const override;
    };
}