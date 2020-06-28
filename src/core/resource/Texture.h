#pragma once

#include <string>

class Texture {
protected:
    int width=1;
    int height=1;
public:
    enum TEXTURE_TYPE {
        RGB8,
        RGBA8,
        DEPTH32,
    };

    virtual void setData(unsigned char *data, int width, int height, TEXTURE_TYPE texture_type) = 0;
    virtual void load(std::string path);

    virtual ~Texture() {};
    virtual void bind(unsigned int slot=0) const=0;
    virtual void unbind(unsigned int slot=0) const=0;
};

