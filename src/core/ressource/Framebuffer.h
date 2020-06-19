#pragma once
#include <core/ressource/Ressource.h>
class Texture;
class Framebuffer: public Ressource{
public:
    enum Type {
        COLOR,
        DEPTH,
        STENCIL,
    };
    virtual void setResolution(int width,int height)=0;
    virtual ~Framebuffer(){};
    virtual void bindTexture() const=0;
    virtual void unbindTexture() const=0;
};


