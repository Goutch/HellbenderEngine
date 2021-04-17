#pragma once

#include "core/graphics/Renderer.h"
#include "vulkan/vulkan.hpp"
#include "VK_Instance.h"


namespace HBE {
    class VK_Window;
    class VK_Renderer : public Renderer {
        VK_Window *window;

        VK_Instance *instance;
    public:
        void render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix) override;

        void present(const RenderTarget *render_target) override;

        void clearDrawCache() override;

        void draw(const Transform &transform, const Mesh &mesh, const Material &material, DRAW_FLAGS flags) override;

        void drawInstanced(const Mesh &mesh, const Material &material, DRAW_FLAGS drawFlags) override;

        void clear() const override;
        VK_Renderer();
        ~VK_Renderer();
    };
}


