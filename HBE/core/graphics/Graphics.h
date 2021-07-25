#pragma once

#include "Core.h"
#include <vector>
#include <string>
#include "DrawFlags.h"
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

    class HB_API Graphics {
        static DRAW_FLAGS default_draw_flags;
        static Renderer *renderer;
        static Window *window;
        static RenderTarget *render_target;
    public:
        static const GraphicPipeline *DEFAULT_SCREEN_PIPELINE;
        static const GraphicPipeline *DEFAULT_MESH_PIPELINE;
        static const Material *DEFAULT_MESH_MATERIAL;
        static const GraphicPipeline *DEFAULT_INSTANCED_PIPELINE;
        static const Mesh *DEFAULT_QUAD;
        static const Mesh *DEFAULT_CUBE;

        /*
         * Initialize graphic context and window
         */
        static void init();

        static Window *getWindow();

        static void
        draw(const Transform &transform, const Mesh &mesh, const Material &material);

        static void drawInstanced(const Mesh &mesh, const Material &material);

        static void render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix);

        static void present(const RenderTarget *render_target);

        static void clear();

        static RenderTarget *getRenderTarget();

        static void terminate();

        static void onWindowSizeChange(int width,int height);
		static void beginFrame();
        static void endFrame();

        static void setDefaultDrawFlags(DRAW_FLAGS drawFlags);

        static DRAW_FLAGS getDefaultDrawFlags();

        static Renderer *getRenderer();

    private:
        static void initializeDefaultVariables();

    };

}