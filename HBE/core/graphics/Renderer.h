#pragma once

#include "vector"
#include "glm/glm.hpp"
#include "Core.h"
using namespace glm;

class RenderTarget;

class Transform;

class Mesh;

class GLFWwindow;

class Framebuffer;

class Material;

class Renderer {
public:
    static Renderer* create();

    virtual ~Renderer() {};

    virtual void init() = 0;

    virtual GLFWwindow *createWindow() = 0;

    virtual void render(const RenderTarget* render_target,const mat4 &projection_matrix, const mat4 &view_matrix = mat4(1.0f)) = 0;

    virtual void present(const RenderTarget* render_target) = 0;

    virtual void clearDrawCache()=0;

    virtual void draw(const Transform &transform, const Mesh &mesh, const Material &material) = 0;

    virtual void drawInstanced(const Mesh &mesh, const Material &material) = 0;

    virtual void clear() const = 0;
};
