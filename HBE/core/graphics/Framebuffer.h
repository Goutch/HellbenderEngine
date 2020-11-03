#pragma once

class Texture;

class Framebuffer {
    friend class RenderTarget;

public:
    enum Type {
        COLOR,
        DEPTH,
        STENCIL,
    };

    static Framebuffer* create();

    virtual void setResolution(int width, int height) {};

    virtual ~Framebuffer() {};

    virtual void bindTexture() const = 0;;

    virtual void unbindTexture() const = 0;

    virtual void bind() const = 0;

    virtual void unbind() const = 0;
};




