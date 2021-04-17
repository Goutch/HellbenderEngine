//
// Created by User on 02-Jun.-2020.
//

#include "GL_Renderer.h"
#include "glad/glad.h"
#include <core/resource/GraphicPipeline.h>
#include <core/resource/Material.h>
#include <core/resource/Mesh.h>
#include <core/entity/component/Transform.h>
#include <Configs.h>
#include "GLFW/glfw3.h"
#include "core/graphics/Framebuffer.h"
#include "core/graphics/RenderTarget.h"
#include "core/utility/Log.h"

#ifdef DEBUG_MODE

#include "GL_Debug.h"

#endif
namespace HBE {
    void GL_Renderer::draw(const Transform &transform, const Mesh &mesh, const Material &material, DRAW_FLAGS draw_flags) {
        auto it_draw_flags = render_cache.try_emplace(draw_flags).first;
        auto it_mesh = it_draw_flags->second.try_emplace(&mesh).first;
        auto it_material = it_mesh->second.try_emplace(&material).first;
        it_material->second.push_back(&transform);
    }

    void GL_Renderer::drawInstanced(const Mesh &mesh, const Material &material, DRAW_FLAGS draw_flags) {
        auto it_draw_flags = render_cache.try_emplace(draw_flags).first;
        auto it_mesh = it_draw_flags->second.try_emplace(&mesh).first;
        auto it_material = it_mesh->second.try_emplace(&material).first;
        it_material->second.push_back(nullptr);
    }

    void GL_Renderer::render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix) {

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glViewport(0, 0, render_target->getWidth(), render_target->getHeight());
        for (auto draw_flags_kv:render_cache) {
            //CULLING
            DRAW_FLAGS draw_flags = draw_flags_kv.first;
            if (!(draw_flags & DRAW_FLAGS_CULL_FACE_FRONT) && !(draw_flags & DRAW_FLAGS_CULL_FACE_BACK)) {
                glDisable(GL_CULL_FACE);
            } else if (draw_flags & DRAW_FLAGS_CULL_FACE_BACK) {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            } else if (draw_flags & DRAW_FLAGS_CULL_FACE_BACK) {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
            }
            for (auto mesh_kv :draw_flags_kv.second) {
                const Mesh *mesh = mesh_kv.first;
                mesh->bind();
                for (auto material_kv:mesh_kv.second) {
                    const Material *material = material_kv.first;
                    material->bind();
                    material->getShader().setUniform("projection_matrix", projection_matrix);
                    material->getShader().setUniform("view_matrix", view_matrix);
                    for (const Transform *transform:material_kv.second) {
                        if (transform)material->getShader().setUniform("transform_matrix", transform->getMatrix());
                        if (mesh->getInstanceCount() == 1) {
                            mesh->hasIndexBuffer() ?
                            glDrawElements(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, 0) :
                            glDrawArrays(GL_TRIANGLES, 0, mesh->getVertexCount());
                        } else if (mesh->getInstanceCount() > 1) {
                            mesh->hasIndexBuffer() ?
                            glDrawElementsInstanced(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, 0, mesh->getInstanceCount())
                                                   :
                            glDrawArraysInstanced(GL_TRIANGLES, 0, mesh->getVertexCount(), mesh->getInstanceCount());
                        }
                    }
                    material->unbind();
                }
                mesh->unbind();
            }
        }
    }

    void GL_Renderer::present(const RenderTarget *render_target) {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glViewport(0, 0, render_target->getWidth(), render_target->getHeight());
        const GraphicPipeline &shader = render_target->getShaderProgram();
        const Framebuffer &framebuffer = render_target->getFramebuffer();
        shader.bind();
        render_target->setShaderUniforms();
        framebuffer.bindTexture();
        Graphics::DEFAULT_QUAD->bind();
        Graphics::DEFAULT_QUAD->hasIndexBuffer() ?
        glDrawElements(GL_TRIANGLES, Graphics::DEFAULT_QUAD->getIndexCount(), GL_UNSIGNED_INT, 0) :
        glDrawArrays(GL_TRIANGLES, 0, Graphics::DEFAULT_QUAD->getVertexCount());
        Graphics::DEFAULT_QUAD->unbind();
        framebuffer.unbindTexture();
        shader.unbind();
    }

    void GL_Renderer::init() {
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            Log::error("Failed to load glad");
        }
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_MULTISAMPLE);
#ifdef DEBUG_MODE
        GL_Debug::init();
#endif
    }


    GLFWwindow *GL_Renderer::createWindow(int32 width, int32 height) {
        if (!glfwInit()) {
            Log::error("Failed to load glfw");
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, Configs::getWindowTitle().c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(window);
        return window;
    }

    void GL_Renderer::clear() const {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void GL_Renderer::clearDrawCache() {
        render_cache.clear();
    }

    GL_Renderer::~GL_Renderer() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }


}




