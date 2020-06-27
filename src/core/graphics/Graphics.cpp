
#include "Graphics.h"
#include <core/entity/Transform.h>
#include <CompilationConfigs.h>
#include <core/utility/Log.h>
#include <GLFW/glfw3.h>
#include <core/utility/Geometry.h>
#include <core/graphics/Layer.h>
#include <core/resource/Resource.h>

#if RENDERER == OPENGL_RENDERER

#include "graphics/gl/GL_Renderer.h"

#elif

#endif
const Mesh *Graphics::DEFAULT_CUBE = nullptr;
const Mesh *Graphics::DEFAULT_QUAD = nullptr;
const ShaderProgram *Graphics::DEFAULT_MESH_SHADER = nullptr;
const ShaderProgram *Graphics::DEFAULT_LAYER_SHADER = nullptr;
IRenderer *Graphics::renderer = nullptr;
GLFWwindow *Graphics::window = nullptr;
Layer *Graphics::default_layer = nullptr;


GLFWwindow *Graphics::init() {
#if RENDERER == OPENGL_RENDERER
    renderer = new GL_Renderer();
#elif RENDERER == VULKAN_RENDERER

#endif

    window = renderer->createWindow();
    renderer->init();
    initializeDefaultVariables();
    return window;
}

void Graphics::draw(const Transform &transform, const Mesh &mesh, const ShaderProgram &shader) {
    renderer->draw(transform, mesh, shader);
}

void Graphics::render(const mat4 &projection_matrix, const mat4 &view_matrix) {
    default_layer->getFramebuffer().bind();
    renderer->clear();
    renderer->render(projection_matrix, view_matrix);
    default_layer->getFramebuffer().unbind();
    renderer->clear();
    renderer->renderLayer(*default_layer);
}

void Graphics::terminate() {
    delete DEFAULT_MESH_SHADER;
    delete DEFAULT_LAYER_SHADER;
    delete DEFAULT_QUAD;
    delete DEFAULT_CUBE;
    delete default_layer;
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
    //DEFAULT_LAYER_SHADER
    ShaderProgram *default_layer_shader = Resource::get<ShaderProgram>();
    default_layer_shader->setShaders(std::string(RESOURCE_PATH) + "shaders/layer.vert",
                                     std::string(RESOURCE_PATH) + "shaders/layer.frag");
    DEFAULT_LAYER_SHADER = default_layer_shader;
    //DEFAULT_LAYER
    default_layer = new Layer(WIDTH, HEIGHT, *DEFAULT_LAYER_SHADER);
}




