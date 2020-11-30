#pragma once

#include "Core.h"
#include <vector>
#include <string>

#include "glm/glm.hpp"
using namespace glm;

struct GLFWwindow;
namespace HBE {
    class Renderer;

    class RenderTarget;

    class ShaderProgram;

    class Mesh;

    class Transform;

    class Material;

    template<typename... Args>
    class Event;

    class HB_API Graphics {
        static Renderer *renderer;
        static GLFWwindow *window;
        static RenderTarget *render_target;
    public:
        static Event<int, int> onWindowSizeChange;
        static const ShaderProgram *DEFAULT_SCREEN_SHADER;
        static const ShaderProgram *DEFAULT_MESH_SHADER;
        static const Material *DEFAULT_MESH_MATERIAL;
        static const ShaderProgram *DEFAULT_INSTANCED_SHADER;
        static const Mesh *DEFAULT_QUAD;
        static const Mesh *DEFAULT_CUBE;

        static GLFWwindow *init();

        static GLFWwindow *getWindow();

        static void draw(const Transform &transform, const Mesh &mesh, const Material &material);

        static void drawInstanced(const Mesh &mesh, const Material &material);

        static void render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix);

        static void present(const RenderTarget *render_target);

        static void clear();

        static RenderTarget *getRenderTarget();

        static void terminate();

        static void getWindowSize(int &width, int &height);

        static void onWindowTitleChange(std::basic_string<char, std::char_traits<char>> title);

        static void onWindowSizeChangeCallback(GLFWwindow *window, int width, int height);

        static void onVerticalSyncChange(bool v_sync);

        static void clearDrawCache();

    private:
        static void initializeDefaultVariables();

    };

}