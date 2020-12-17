#pragma once

#include <core/graphics/Renderer.h>
#include "unordered_map"

namespace HBE {

#define MAP(T1,T2) std::unordered_map<T1,T2>

    class ShaderProgram;

    class Transform;

    class Mesh;
    class GL_Renderer : public Renderer {
    private:
        struct RenderObject {
            const Transform *transform;
            const Mesh *mesh;
            const Material *material;
            const DRAW_FLAGS draw_flags;
        };
        MAP(DRAW_FLAGS,MAP(const Mesh*,MAP(const Material*,std::list<const Transform*>))) render_cache;
    private:
        GLFWwindow *window;
    public:
        void clear() const override;
        ~GL_Renderer() override;
        void init() override;

        void draw(const Transform &transform, const Mesh &mesh, const Material &material, DRAW_FLAGS draw_flags) override;

        void drawInstanced(const Mesh &mesh, const Material &material, DRAW_FLAGS draw_flags) override;

        void render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix) override;

        void present(const RenderTarget *render_target) override;

        void clearDrawCache() override;

        GLFWwindow *createWindow(int32 width, int32 height) override;
    };

}