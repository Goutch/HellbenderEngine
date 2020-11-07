#pragma once


#include <vector>
#include <string>
#include "glm/glm.hpp"
using namespace glm;
class Renderer;
class GLFWwindow;
class RenderTarget;
class ShaderProgram;
class Mesh;
class Transform;
class Material;

class Graphics {
    static Renderer* renderer;
    static GLFWwindow *window;
    static RenderTarget* render_target;
public:
    static const ShaderProgram* DEFAULT_SCREEN_SHADER;
    static const ShaderProgram* DEFAULT_MESH_SHADER;
    static const ShaderProgram* DEFAULT_INSTANCED_SHADER;
    static const Mesh* DEFAULT_QUAD;
    static const Mesh* DEFAULT_CUBE;
    static GLFWwindow *init();
    static GLFWwindow *getWindow();
    static void draw(const Transform &transform, const Mesh &mesh, const Material &material);
    static void drawInstanced(const Mesh &mesh, const Material &material);
    static void render(const mat4& projection_matrix,const mat4& view_matrix);
    static RenderTarget* getRenderTarget();
    static void terminate();
    static void onWindowTitleChange(std::basic_string<char, std::char_traits<char>> title);
    static void onWindowSizeChange(GLFWwindow* window,int width,int height);
    static void onVerticalSyncChange(bool v_sync);
private:
    static void initializeDefaultVariables();

};

