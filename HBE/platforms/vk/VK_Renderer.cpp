

#include "VK_Renderer.h"

#define GLFW_INCLUDE_VULKAN

#include "Configs.h"
#include "VK_Window.h"

namespace HBE {
    VK_Renderer::VK_Renderer() {

        window = dynamic_cast<VK_Window*>(Graphics::getWindow());
        instance = new VK_Instance();
    }


    VK_Renderer::~VK_Renderer() {
        delete instance;
    }

    void VK_Renderer::render(const HBE::RenderTarget *render_target,
                             const mat4 &projection_matrix,
                             const mat4 &view_matrix) {

    }

    void VK_Renderer::present(const HBE::RenderTarget *render_target) {

    }

    void VK_Renderer::clearDrawCache() {

    }

    void VK_Renderer::draw(const HBE::Transform &transform,
                           const HBE::Mesh &mesh,
                           const HBE::Material &material,
                           HBE::DRAW_FLAGS flags) {

    }

    void VK_Renderer::drawInstanced(const HBE::Mesh &mesh,
                                    const HBE::Material &material,
                                    HBE::DRAW_FLAGS drawFlags) {

    }

    void VK_Renderer::clear() const {

    }


}

