
#include "Graphics.h"
#include <core/entity/Transform.h>
#include <CompilationConfigs.h>
#include <core/utility/Log.h>
#include <GLFW/glfw3.h>
#include <core/utility/Geometry.h>
#include <core/graphics/RenderTarget.h>
#include <Configs.h>
#include "Framebuffer.h"
#include "core/resource/ShaderProgram.h"
#if RENDERER == OPENGL_RENDERER

#include "graphics/gl/GL_Renderer.h"

#elif

#endif

const Mesh *Graphics::DEFAULT_CUBE = nullptr;
const Mesh *Graphics::DEFAULT_QUAD = nullptr;
const ShaderProgram *Graphics::DEFAULT_MESH_SHADER = nullptr;
const ShaderProgram *Graphics::DEFAULT_SCREEN_SHADER = nullptr;
const ShaderProgram *Graphics::DEFAULT_INSTANCED_SHADER = nullptr;
Renderer *Graphics::renderer = nullptr;
GLFWwindow *Graphics::window = nullptr;
RenderTarget *Graphics::render_target = nullptr;

GLFWwindow *Graphics::init() {
    renderer = Renderer::create();
    window = renderer->createWindow();

    if (!Configs::getVerticalSync())
        glfwSwapInterval(0);

    renderer->init();
    glfwSetWindowSizeCallback(window, Graphics::onWindowSizeChange);
    initializeDefaultVariables();
    Configs::onVerticalSyncChange.subscribe(Graphics::onVerticalSyncChange);
    return window;
}

void Graphics::onVerticalSyncChange(bool v_sync) {
    glfwSwapInterval(v_sync);
}

void Graphics::onWindowSizeChange(GLFWwindow *window, int width, int height) {
    render_target->setSize(width, height);
}

void Graphics::draw(const Transform &transform, const Mesh &mesh, const Material &material) {
    renderer->draw(transform, mesh, material);
}

void Graphics::drawInstanced(const Mesh &mesh, const Material &material) {
    renderer->drawInstanced(mesh, material);
}

void Graphics::render(const mat4 &projection_matrix, const mat4 &view_matrix) {
    render_target->getFramebuffer().bind();
    renderer->clear();
    renderer->render(projection_matrix, view_matrix);
    render_target->getFramebuffer().unbind();
    renderer->clear();
    renderer->renderTarget(*render_target);
}

void Graphics::terminate() {
    Configs::onVerticalSyncChange.unsubscribe(Graphics::onVerticalSyncChange);
    delete DEFAULT_MESH_SHADER;
    delete DEFAULT_SCREEN_SHADER;
    delete DEFAULT_INSTANCED_SHADER;
    delete DEFAULT_QUAD;
    delete DEFAULT_CUBE;
    delete render_target;
    delete renderer;
    glfwTerminate();
}

void Graphics::initializeDefaultVariables() {
    //DEFAULT_CUBE
    Mesh *cube = Mesh::create();
    Geometry::createCube(*cube, 1, 1, 1);
    DEFAULT_CUBE = cube;
    //DEFAULT_QUAD
    Mesh *quad = Mesh::create();
    Geometry::createQuad(*quad, 1, 1);
    DEFAULT_QUAD = quad;
    //DEFAULT_MESH_SHADER
    ShaderProgram *default_mesh_shader = ShaderProgram::create();
    default_mesh_shader->setShaders(std::string(RESOURCE_PATH) + "shaders/shader.vert",
                                    std::string(RESOURCE_PATH) + "shaders/shader.frag");
    DEFAULT_MESH_SHADER = default_mesh_shader;
    //DEFAULT_INSTANCED_SHADER
    ShaderProgram *default_instanced_shader = ShaderProgram::create();
    default_instanced_shader->setShaders(std::string(RESOURCE_PATH) + "shaders/instancedShader.vert",
                                         std::string(RESOURCE_PATH) + "shaders/shader.frag");
    DEFAULT_INSTANCED_SHADER = default_instanced_shader;
    //DEFAULT_SCREEN_SHADER
    ShaderProgram *default_screen_shader = ShaderProgram::create();
    default_screen_shader->setShaders(std::string(RESOURCE_PATH) + "shaders/screen.vert",
                                      std::string(RESOURCE_PATH) + "shaders/screen.frag");
    DEFAULT_SCREEN_SHADER = default_screen_shader;
    //DEFAULT_RENDER_TARGET
    render_target = new RenderTarget(WIDTH, HEIGHT, *DEFAULT_SCREEN_SHADER);
}

RenderTarget *Graphics::getRenderTarget() {
    return render_target;
}










