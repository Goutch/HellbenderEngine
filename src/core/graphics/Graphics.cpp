
#include "Graphics.h"
#include "graphics/gl/GL_Renderer.h"
#include <core/ressource/ShaderProgram.h>
#include <core/ressource/Mesh.h>
#include <core/entity/Transform.h>
#include <Configs.h>
#include "core/utility/Log.h"
#include <GLFW/glfw3.h>
#include <core/utility/Geometry.h>
#include "core/graphics/Layer.h"

#include <graphics/gl/GL_Mesh.h>
#include "graphics/gl/GL_ShaderProgram.h"
const Mesh* Graphics::DEFAULT_QUAD= nullptr;
const ShaderProgram* Graphics::DEFAULT_MESH_SHADER= nullptr;
const ShaderProgram* Graphics::DEFAULT_LAYER_SHADER= nullptr;
Renderer* Graphics::renderer= nullptr;
GLFWwindow* Graphics::window= nullptr;
Layer* Graphics::default_layer= nullptr;



GLFWwindow* Graphics::init() {
    renderer=new GL_Renderer();
    window=renderer->createWindow();
    renderer->init();

    initializeDefaultVariables();
    return window;
}

void Graphics::draw(const Transform &transform, const Mesh &mesh, const ShaderProgram &shader) {
    renderer->draw(transform, mesh, shader);
}

void Graphics::render(const mat4 &projection_matrix, const mat4& view_matrix) {
    default_layer->getFramebuffer().bind();
    renderer->clear();
    renderer->render(projection_matrix,view_matrix);
    default_layer->getFramebuffer().unbind();
    renderer->clear();
    renderer->renderLayer(*default_layer);
}

void Graphics::terminate() {
    delete DEFAULT_MESH_SHADER;
    delete DEFAULT_LAYER_SHADER;
    delete DEFAULT_QUAD;
    delete default_layer;
    delete renderer;
}

void Graphics::initializeDefaultVariables() {
    Mesh* quad=new GL_Mesh();
    Geometry::createQuad(*quad,1,1);
    DEFAULT_QUAD=quad;
    DEFAULT_MESH_SHADER=new GL_ShaderProgram(std::string(RESSOURCE_PATH) + "shaders/shader.vert",
                                        std::string(RESSOURCE_PATH) + "shaders/shader.frag");

    DEFAULT_LAYER_SHADER=new GL_ShaderProgram(std::string(RESSOURCE_PATH) + "shaders/layer.vert",
                                              std::string(RESSOURCE_PATH) + "shaders/layer.frag");
    default_layer=new Layer(WIDTH, HEIGHT, *DEFAULT_LAYER_SHADER);
}




