#pragma once


#include <core/ressource/Framebuffer.h>

class GL_MultisamplingFramebuffer: public Framebuffer {
    unsigned int ms_fbo;
    unsigned int fbo;
    unsigned int rbo;
    int width,height;
    unsigned int ms_texture;
    unsigned int texture;
public:
    GL_MultisamplingFramebuffer(int width,int height);
    ~GL_MultisamplingFramebuffer() override;

    void setResolution(int width,int height) override;
    void bindTexture() const override ;
    void unbindTexture() const override ;
    void bind() const;
    void unbind() const;
};

