#pragma once

#include <core/resource/Texture.h>

class GL_Texture : public Texture {

private:
    unsigned int texture_id;
public:
    GL_Texture();

    ~GL_Texture();

    void setData(unsigned char *data, int width, int height, TEXTURE_TYPE texture_type) override;

    void bind(unsigned int slot=0) const override ;

    void unbind(unsigned int slot=0) const override ;

    unsigned int getId();
};
