#pragma once


#include <vector>
#include <core/ressource/Framebuffer.h>
#include "Renderer.h"


class Graphics {
    static Renderer* renderer;
    static GLFWwindow *window;
    static Layer* default_layer;
public:
    static const ShaderProgram* DEFAULT_LAYER_SHADER;
    static const ShaderProgram* DEFAULT_MESH_SHADER;
    static const Mesh* DEFAULT_QUAD;
    static GLFWwindow *init();
    static void draw(const Transform &transform, const Mesh &mesh, const ShaderProgram &shader);
    static void render(const mat4& projection_matrix,const mat4& view_matrix);
    static void terminate();

private:
    static void initializeDefaultVariables();


};

