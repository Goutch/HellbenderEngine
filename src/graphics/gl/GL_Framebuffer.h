#pragma once

#include <vector>
#include <core/ressource/Framebuffer.h>
#include "GL_Texture.h"

class GL_Framebuffer: public Framebuffer {

    unsigned int fbo;
    unsigned int rbo;
    int width,height;

    unsigned int texture;
public:
    GL_Framebuffer(int width,int height);
    ~GL_Framebuffer();

    void setResolution(int width,int height) override;
    void bindTexture() const override ;
    void unbindTexture() const override ;
    void bind() const;
    void unbind() const;
};
