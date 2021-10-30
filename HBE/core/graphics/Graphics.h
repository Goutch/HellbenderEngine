#pragma once

#include "Core.h"
#include <vector>
#include <string>
#include "DrawFlags.h"
#include "glm/glm.hpp"


namespace HBE {
    class Renderer;

    class RenderTarget;

    class GraphicPipeline;

    class Mesh;

    class Transform;

    class Window;

    template<typename... Args>
    class Event;

    class HB_API Graphics {
        static DRAW_FLAGS default_draw_flags;
        static Renderer *renderer;
        static Window *window;
        static RenderTarget *main_render_target;
    public:
        static const Mesh *DEFAULT_QUAD;
        static const Mesh *DEFAULT_CUBE;

        static void init();

        static Window *getWindow();

        static void
        draw(const Transform &transform, const Mesh &mesh, GraphicPipeline &material);

        static void drawInstanced(const Mesh &mesh, GraphicPipeline &pipeline);

        static void render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix);

        static void present(const RenderTarget *render_target);

        static void clear();

        static const RenderTarget *getRenderTarget();

        static void terminate();

        static void onWindowSizeChange(uint32_t width,uint32_t height);

		static void beginFrame();

        static void endFrame();

        static void setDefaultDrawFlags(DRAW_FLAGS drawFlags);

        static DRAW_FLAGS getDefaultDrawFlags();

        static Renderer *getRenderer();

    private:
        static void initializeDefaultVariables();

    };

}