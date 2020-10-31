
#include "Graphics.h"
#include <core/entity/Transform.h>
#include <CompilationConfigs.h>
#include <core/utility/Log.h>
#include <GLFW/glfw3.h>
#include <core/utility/Geometry.h>
#include <core/graphics/RenderTarget.h>
#include <core/resource/Resource.h>
#include <Configs.h>

#if RENDERER == OPENGL_RENDERER

#include "graphics/gl/GL_Renderer.h"

#elif

#endif

const Mesh *Graphics::DEFAULT_CUBE = nullptr;
const Mesh *Graphics::DEFAULT_QUAD = nullptr;
const ShaderProgram *Graphics::DEFAULT_MESH_SHADER = nullptr;
const ShaderProgram *Graphics::DEFAULT_SCREEN_SHADER = nullptr;
const ShaderProgram *Graphics::DEFAULT_INSTANCED_SHADER = nullptr;
IRenderer *Graphics::renderer = nullptr;
GLFWwindow *Graphics::window = nullptr;
RenderTarget *Graphics::render_target = nullptr;


GLFWwindow *Graphics::init() {
#if RENDERER == OPENGL_RENDERER
    renderer = new GL_Renderer();
#elif RENDERER == VULKAN_RENDERER

#endif
    window = renderer->createWindow();

    if (!Configs::getVerticalSync())
        glfwSwapInterval(0);
    renderer->init();
    glfwSetWindowSizeCallback(window,Graphics::onWindowSizeChange);
    initializeDefaultVariables();

    return window;
}

void Graphics::onWindowSizeChange(GLFWwindow* window, int width, int height)
{
    render_target->setSize(width,height);
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
    renderer->renderLayer(*render_target);
}

void Graphics::terminate() {

    delete DEFAULT_MESH_SHADER;
    delete DEFAULT_SCREEN_SHADER;
    delete DEFAULT_INSTANCED_SHADER;
    delete DEFAULT_QUAD;
    delete DEFAULT_CUBE;
    delete render_target;
    delete renderer;
}

void Graphics::initializeDefaultVariables() {
    //DEFAULT_CUBE
    Mesh *cube = Resource::get<Mesh>();
    Geometry::createCube(*cube, 1, 1, 1);
    DEFAULT_CUBE = cube;
    //DEFAULT_QUAD
    Mesh *quad = Resource::get<Mesh>();
    Geometry::createQuad(*quad, 1, 1);
    DEFAULT_QUAD = quad;
    //DEFAULT_MESH_SHADER
    ShaderProgram *default_mesh_shader = Resource::get<ShaderProgram>();
    default_mesh_shader->setShaders(std::string(RESOURCE_PATH) + "shaders/shader.vert",
                                    std::string(RESOURCE_PATH) + "shaders/shader.frag");
    DEFAULT_MESH_SHADER = default_mesh_shader;
    //DEFAULT_INSTANCED_SHADER
    ShaderProgram *default_instanced_shader = Resource::get<ShaderProgram>();
    default_instanced_shader->setShaders(std::string(RESOURCE_PATH) + "shaders/instancedShader.vert",
                                         std::string(RESOURCE_PATH) + "shaders/shader.frag");
    DEFAULT_INSTANCED_SHADER = default_instanced_shader;
    //DEFAULT_SCREEN_SHADER
    ShaderProgram *default_screen_shader = Resource::get<ShaderProgram>();
    default_screen_shader->setShaders(std::string(RESOURCE_PATH) + "shaders/screen.vert",
                                     std::string(RESOURCE_PATH) + "shaders/screen.frag");
    DEFAULT_SCREEN_SHADER = default_screen_shader;
    //DEFAULT_RENDER_TARGET
    render_target = new RenderTarget(WIDTH, HEIGHT, *DEFAULT_SCREEN_SHADER);
}

RenderTarget *Graphics::getRenderTarget() {
    return render_target;
}










