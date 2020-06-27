#pragma once


#include <vector>
#include "glm/glm.hpp"
using namespace glm;
class IRenderer;
class GLFWwindow;
class Layer;
class ShaderProgram;
class Mesh;
class Transform;


class Graphics {
    static IRenderer* renderer;
    static GLFWwindow *window;
    static Layer* default_layer;
public:
    static const ShaderProgram* DEFAULT_LAYER_SHADER;
    static const ShaderProgram* DEFAULT_MESH_SHADER;
    static const ShaderProgram* DEFAULT_INSTANCED_SHADER;
    static const Mesh* DEFAULT_QUAD;
    static const Mesh* DEFAULT_CUBE;
    static GLFWwindow *init();
    static void draw(const Transform &transform, const Mesh &mesh, const ShaderProgram &shader);
    static void drawInstanced(const Mesh &mesh, const ShaderProgram &shader);
    static void render(const mat4& projection_matrix,const mat4& view_matrix);
    static void terminate();



private:
    static void initializeDefaultVariables();


};

