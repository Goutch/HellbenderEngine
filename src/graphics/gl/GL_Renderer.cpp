//
// Created by User on 02-Jun.-2020.
//

#include "GL_Renderer.h"
#include "glad/glad.h"
#include <core/resource/ShaderProgram.h>
#include <core/resource/Material.h>
#include <core/resource/Mesh.h>
#include <core/entity/Transform.h>
#include "GLFW/glfw3.h"
#include "core/utility/Log.h"
#include "CompilationConfigs.h"
#include "core/resource/Framebuffer.h"
#include "core/graphics/Layer.h"

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

void GL_Renderer::render(const mat4 &projection_matrix, const mat4 &view_matrix) {
    glEnable(GL_DEPTH_TEST);
    for (auto ro:render_objects) {
        ro.material->bind();
        ro.material->getShader().setUniform("projection_matrix", projection_matrix);
        ro.material->getShader().setUniform("view_matrix", view_matrix);
        if (ro.transform)
            ro.material->getShader().setUniform("transform_matrix", ro.transform->getMatrix());
        ro.mesh->bind();
        if (ro.mesh->getInstanceCount() == 1) {
            ro.mesh->hasIndexBuffer() ?
            glDrawElements(GL_TRIANGLES, ro.mesh->getIndexCount(), GL_UNSIGNED_INT, 0) :
            glDrawArrays(GL_TRIANGLES, 0, ro.mesh->getVertexCount());
        } else if (ro.mesh->getInstanceCount() > 1) {
            ro.mesh->hasIndexBuffer() ?
            glDrawElementsInstanced(GL_TRIANGLES, ro.mesh->getIndexCount(), GL_UNSIGNED_INT, 0, ro.mesh->getInstanceCount()) :
            glDrawArraysInstanced(GL_TRIANGLES, 0, ro.mesh->getVertexCount(), ro.mesh->getInstanceCount());
        }

        ro.mesh->unbind();
        ro.material->unbind();
    }
    render_objects.clear();
}

void GL_Renderer::renderLayer(Layer &layer) {
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, layer.getWidth(), layer.getHeight());
    const ShaderProgram &shader = layer.getShaderProgram();
    const Framebuffer &framebuffer = layer.getFramebuffer();
    shader.bind();
    layer.setShaderUniforms();
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
    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
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

    window = glfwCreateWindow(WIDTH, HEIGHT, APPLICATION_NAME, nullptr, nullptr);
    glfwMakeContextCurrent(window);
    return window;
}

void GL_Renderer::clear() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}







