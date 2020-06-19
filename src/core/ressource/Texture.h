#pragma once

#include <string>
#include <core/ressource/Ressource.h>

class Texture : public Ressource {
public:
    enum TEXTURE_TYPE {
        RGB8,
        RGBA8,
        DEPTH32,
    };

    virtual void setData(unsigned char *data, int width, int height, TEXTURE_TYPE texture_type) = 0;

    virtual void setSlot(unsigned int slot) = 0;

    virtual ~Texture() {};
};

