
#include "GL_Renderer.h"
#include "glad/glad.h"
#include <core/resource/GraphicPipeline.h>
#include <core/resource/GraphicPipelineInstance.h>
#include <core/resource/Mesh.h>
#include <core/entity/component/Transform.h>
#include <Configs.h>
#include "GLFW/glfw3.h"
#include "core/graphics/Framebuffer.h"
#include "HBE/core/resource/RenderTarget.h"
#include "core/utility/Log.h"
#include "GL_ResourceFactory.h"

#ifdef DEBUG_MODE

#include "GL_Debug.h"

#endif


namespace HBE {
    void
    GL_Renderer::draw(const Transform &transform, const Mesh &mesh, const Material &material) {
        auto it_material = render_cache.try_emplace(&material).first;
        auto it_mesh = it_material->second.try_emplace(&mesh).first;
        it_mesh->second.push_back(&transform);
    }

    void GL_Renderer::drawInstanced(const Mesh &mesh, const Material &material) {
        auto it_material = render_cache.try_emplace(&material).first;
        auto it_mesh = it_material->second.try_emplace(&mesh).first;
        it_mesh->second.push_back(nullptr);
    }

    void
    GL_Renderer::render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix) {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glViewport(0, 0, render_target->getWidth(), render_target->getHeight());


        for (auto material_kv:render_cache) {
            const Material *material = material_kv.first;
            const GraphicPipeline *pipeline = material->getPipeline();
            material->bind();

            pipeline->setUniform("projection_matrix", projection_matrix);
            pipeline->setUniform("view_matrix", view_matrix);

            DRAW_FLAGS draw_flags = pipeline->getDrawFlags();
            if (!(draw_flags & DRAW_FLAGS_CULL_FACE_FRONT) && !(draw_flags & DRAW_FLAGS_CULL_FACE_BACK)) {
                glDisable(GL_CULL_FACE);
            } else if (draw_flags & DRAW_FLAGS_CULL_FACE_BACK) {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            } else if (draw_flags & DRAW_FLAGS_CULL_FACE_BACK) {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
            }
            for (auto mesh_kv :material_kv.second) {
                const Mesh *mesh = mesh_kv.first;
                mesh->bind();
                for (const Transform *transform:mesh_kv.second) {
                    if (transform)pipeline->setUniform("transform_matrix", transform->getMatrix());
                    if (mesh->getInstanceCount() == 1) {
                        mesh->hasIndexBuffer() ?
                        glDrawElements(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, 0) :
                        glDrawArrays(GL_TRIANGLES, 0, mesh->getVertexCount());
                    } else if (mesh->getInstanceCount() > 1) {
                        mesh->hasIndexBuffer() ?
                        glDrawElementsInstanced(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, 0,
                                                mesh->getInstanceCount())
                                               :
                        glDrawArraysInstanced(GL_TRIANGLES, 0, mesh->getVertexCount(), mesh->getInstanceCount());
                    }
                }
                mesh->unbind();

            }
            material->unbind();
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

    GL_Renderer::GL_Renderer() {
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            Log::error("Failed to load glad");
        }
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_MULTISAMPLE);
#ifdef DEBUG_MODE
        GL_Debug::init();
#endif
        factory = new GL_ResourceFactory();
    }


    void GL_Renderer::clear() const {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void GL_Renderer::clearDrawCache() {
        render_cache.clear();
    }

    GL_Renderer::~GL_Renderer() {
        delete factory;
    }

    const IResourceFactory *GL_Renderer::getResourceFactory() const {
        return factory;
    }

}




