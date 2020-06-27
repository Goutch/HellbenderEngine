#pragma once

#include "vector"
#include "glm/glm.hpp"
using namespace glm;
class Layer;
class ShaderProgram;
class Transform;
class Mesh;
class GLFWwindow;
class Framebuffer;
class IRenderer {
public:
    virtual ~IRenderer(){};
    virtual void init()=0;
    virtual GLFWwindow* createWindow()=0;
    virtual void render(const mat4& projection_matrix,const mat4& view_matrix)=0;
    virtual void renderLayer(Layer &layer) =0;
    virtual void draw(const Transform &transform, const Mesh &mesh, const ShaderProgram &shader) =0;
    virtual void drawInstanced(const Mesh &mesh, const ShaderProgram &shader) =0;
    virtual void clear()const =0;
};
