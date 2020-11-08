//
// Created by User on 02-Jun.-2020.
//

#include "GL_Renderer.h"
#include "glad/glad.h"
#include <core/resource/ShaderProgram.h>
#include <core/resource/Material.h>
#include <core/resource/Mesh.h>
#include <core/entity/Transform.h>
#include <Configs.h>
#include "GLFW/glfw3.h"
#include "CompilationConfigs.h"
#include "core/graphics/Framebuffer.h"
#include "core/graphics/RenderTarget.h"

#if DEBUG_MODE

#include "GL_Debug.h"

#endif

void
GL_Renderer::draw(const Transform &transform, const Mesh &mesh, const Material &material) {
    render_objects.push_back({&transform, &mesh, &material});
}

void GL_Renderer::drawInstanced(const Mesh &mesh, const Material &material) {
    render_objects.push_back({nullptr, &mesh, &material});
}

void GL_Renderer::render(const RenderTarget* render_target,const mat4 &projection_matrix, const mat4 &view_matrix) {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glViewport(0, 0, render_target->getWidth(), render_target->getHeight());
    for (auto render_object:render_objects) {
        render_object.material->bind();
        render_object.material->getShader().setUniform("projection_matrix", projection_matrix);
        render_object.material->getShader().setUniform("view_matrix", view_matrix);
        if (render_object.transform)
            render_object.material->getShader().setUniform("transform_matrix", render_object.transform->getMatrix());
        render_object.mesh->bind();
        if (render_object.mesh->getInstanceCount() == 1) {
            render_object.mesh->hasIndexBuffer() ?
            glDrawElements(GL_TRIANGLES, render_object.mesh->getIndexCount(), GL_UNSIGNED_INT, 0) :
            glDrawArrays(GL_TRIANGLES, 0, render_object.mesh->getVertexCount());
        } else if (render_object.mesh->getInstanceCount() > 1) {
            render_object.mesh->hasIndexBuffer() ?
            glDrawElementsInstanced(GL_TRIANGLES, render_object.mesh->getIndexCount(), GL_UNSIGNED_INT, 0, render_object.mesh->getInstanceCount()) :
            glDrawArraysInstanced(GL_TRIANGLES, 0, render_object.mesh->getVertexCount(), render_object.mesh->getInstanceCount());
        }

        render_object.mesh->unbind();
        render_object.material->unbind();
    }
}

void GL_Renderer::present(const RenderTarget *render_target) {
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, render_target->getWidth(), render_target->getHeight());
    const ShaderProgram &shader = render_target->getShaderProgram();
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
    int w,h;
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
#if DEBUG_MODE
    GL_Debug::init();
#endif
}



GLFWwindow *GL_Renderer::createWindow() {
    if (!glfwInit()) {
        Log::error("Failed to load glfw");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, Configs::getWindowTitle().c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window);
    return window;
}

void GL_Renderer::clear() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GL_Renderer::clearDrawCache() {
    render_objects.clear();
}







