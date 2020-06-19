#pragma once

#include <core/ressource/Texture.h>

class GL_Texture : public Texture {
public:
    void setSlot(unsigned int slot) override;

private:
    unsigned int slot = 0;
    unsigned int texture_id;
public:
    GL_Texture();

    ~GL_Texture();

    void setData(unsigned char *data, int width, int height, TEXTURE_TYPE texture_type) override;

    void bind() const;

    void unbind() const;

    unsigned int getId();
};
