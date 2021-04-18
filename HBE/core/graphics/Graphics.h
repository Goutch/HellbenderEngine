#pragma once

#include "Core.h"
#include <vector>
#include <string>

#include "glm/glm.hpp"

using namespace glm;

namespace HBE {
    class Renderer;

    class RenderTarget;

    class GraphicPipeline;

    class Mesh;

    class Transform;

    class Material;

    class Window;

    template<typename... Args>
    class Event;

    enum {
        DRAW_FLAGS_NONE = 0,
        DRAW_FLAGS_CULL_FACE_BACK = 1,
        DRAW_FLAGS_CULL_FACE_FRONT = 1 << 1,
        DRAW_FLAGS_TRANSPARENT = 1 << 2,
    };
    typedef uint32 DRAW_FLAGS;

    class HB_API Graphics {
        static DRAW_FLAGS default_draw_Flags;
        static Renderer *renderer;
        static Window *window;
        static RenderTarget *render_target;
    public:
        static const GraphicPipeline *DEFAULT_SCREEN_SHADER_PROGRAM;
        static const GraphicPipeline *DEFAULT_MESH_SHADER_PROGRAM;
        static const Material *DEFAULT_MESH_MATERIAL;
        static const GraphicPipeline *DEFAULT_INSTANCED_SHADER_PROGRAM;
        static const Mesh *DEFAULT_QUAD;
        static const Mesh *DEFAULT_CUBE;

        /*
         * Initialze graphic context and window
         */
        static void init();

        static Window *getWindow();

        static void
        draw(const Transform &transform, const Mesh &mesh, const Material &material, DRAW_FLAGS = default_draw_Flags);

        static void drawInstanced(const Mesh &mesh, const Material &material, DRAW_FLAGS = default_draw_Flags);

        static void render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix);

        static void present(const RenderTarget *render_target);

        static void clear();

        static RenderTarget *getRenderTarget();

        static void terminate();

        static void onWindowSizeChange(int width,int height);

        static void clearDrawCache();

        static void setDefaultDrawFlags(DRAW_FLAGS drawFlags);

        static Renderer *getRenderer();

    private:
        static void initializeDefaultVariables();

    };

}