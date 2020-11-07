#pragma once

#include <vector>
#include <core/graphics/Framebuffer.h>
#include "GL_Texture.h"

class GL_Framebuffer: public Framebuffer {

    unsigned int fbo;
    unsigned int rbo;
    int width,height;

    unsigned int texture;
public:
    GL_Framebuffer();
    GL_Framebuffer(int width,int height);


    ~GL_Framebuffer();

    unsigned int getTextureID() override;

    void setResolution(int width,int height) override;
    void bindTexture() const override ;
    void unbindTexture() const override ;
    void bind() const;
    void unbind() const;
};
