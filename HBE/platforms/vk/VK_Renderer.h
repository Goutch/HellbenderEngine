#pragma once
#include "core/graphics/Renderer.h"
#include "vulkan/vulkan.hpp"
namespace HBE{
    class VK_Renderer: public Renderer {
        GLFWwindow* window;

        VkInstance instance;
    public:

        void render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix) override;

        void present(const RenderTarget *render_target) override;

        void clearDrawCache() override;

        void draw(const Transform &transform, const Mesh &mesh, const Material &material, DRAW_FLAGS flags) override;

        void drawInstanced(const Mesh &mesh, const Material &material, DRAW_FLAGS drawFlags) override;

        void clear() const override;

        ~VK_Renderer();
        GLFWwindow *createWindow(int32 width, int32 height) override;

        void init() override;
    };
}


