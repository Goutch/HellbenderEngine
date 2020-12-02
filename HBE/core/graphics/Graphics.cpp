
#include "Graphics.h"

#include "Framebuffer.h"
#include <core/entity/component/Transform.h>
#include <core/utility/Log.h>
#include <core/utility/Event.h>
#include <core/utility/Geometry.h>
#include <core/graphics/RenderTarget.h>
#include "core/resource/ShaderProgram.h"
#include <Configs.h>
#include <GLFW/glfw3.h>

#if RENDERER == OPENGL_RENDERER

#include "platforms/gl/GL_Renderer.h"

#elif

#endif

#include "core/resource/Material.h"

namespace HBE {
    const Mesh *Graphics::DEFAULT_CUBE = nullptr;
    const Mesh *Graphics::DEFAULT_QUAD = nullptr;
    const ShaderProgram *Graphics::DEFAULT_MESH_SHADER = nullptr;
    const ShaderProgram *Graphics::DEFAULT_SCREEN_SHADER = nullptr;
    const ShaderProgram *Graphics::DEFAULT_INSTANCED_SHADER = nullptr;
    const Material *Graphics::DEFAULT_MESH_MATERIAL = nullptr;
    Renderer *Graphics::renderer = nullptr;
    GLFWwindow *Graphics::window = nullptr;
    RenderTarget *Graphics::render_target = nullptr;
    Event<int, int> Graphics::onWindowSizeChange;


    const char *default_mesh_vertex_shader_code = R"(#version 330 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_uvs;
out vec2 uv;

uniform mat4 projection_matrix;
uniform mat4 transform_matrix;
uniform mat4 view_matrix;

void main()
{
    uv=vertex_uvs;

    gl_Position = projection_matrix*view_matrix*transform_matrix*vec4(vertex_position, 1.0);
})";

    const char *default_mesh_fragment_shader_code = R"(#version 330 core
out vec4 FragColor;
in vec2 uv;
uniform bool has_texture;
uniform sampler2D texture_0;
uniform vec4 material_color;
void main()
{
    if(has_texture)
    {
        FragColor=texture(texture_0,uv)*material_color;
    }
    else
    {
        FragColor =material_color;
    }
})";

    const char *default_screen_vertex_shader_code = R"(#version 330 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_uvs;

out vec2 uvs;
uniform mat4 projection_matrix;

void main()
{
    uvs=vertex_uvs;
    gl_Position = projection_matrix*vec4(vertex_position, 1.0);
})";
    const char *default_screen_fragment_shader_code = R"(#version 330 core
out vec4 FragColor;
in vec2 uvs;
uniform vec2 resolution;
uniform sampler2D texture_0;
const float exposure=1.;
const float gamma=1.;

void main()
{
    vec4 color=texture(texture_0, uvs);

    FragColor = vec4(pow(vec3(1.0) - exp(-color.rgb * exposure), vec3(1.0 / gamma)), color.a);
})";


    const char *default_instanced_vertex_shader_code = R"(#version 330 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_uvs;
layout (location = 3) in mat4 instance_transform;
out vec2 uvs;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;

void main()
{
    uvs=vertex_uvs;
    gl_Position = projection_matrix*view_matrix*instance_transform*vec4(vertex_position, 1.0);
})";

    GLFWwindow *Graphics::init() {
        renderer = Renderer::create();
        window = renderer->createWindow(900, 600);

        if (!Configs::getVerticalSync())
            glfwSwapInterval(0);
        Configs::onVerticalSyncChange.subscribe(Graphics::onVerticalSyncChange);
        Configs::onWindowTitleChange.subscribe(&Graphics::onWindowTitleChange);
        renderer->init();

        glfwSetWindowSizeCallback(window, Graphics::onWindowSizeChangeCallback);
        initializeDefaultVariables();

        return window;
    }

    void Graphics::onVerticalSyncChange(bool v_sync) {
        glfwSwapInterval(v_sync);
    }

    void Graphics::onWindowSizeChangeCallback(GLFWwindow *window, int width, int height) {
        if (!Configs::isCustonRenderingOn()) {
            render_target->setSize(width, height);
        }
        onWindowSizeChange.invoke(width, height);
    }

    void Graphics::draw(const Transform &transform, const Mesh &mesh, const Material &material) {
        renderer->draw(transform, mesh, material);
    }

    void Graphics::drawInstanced(const Mesh &mesh, const Material &material) {
        renderer->drawInstanced(mesh, material);
    }

    void Graphics::render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix) {
        render_target->getFramebuffer().bind();
        renderer->clear();
        renderer->render(render_target, projection_matrix, view_matrix);
        render_target->getFramebuffer().unbind();

    }

    void Graphics::present(const RenderTarget *render_target) {
        renderer->clear();
        renderer->present(render_target);
    }

    void Graphics::terminate() {
        Configs::onVerticalSyncChange.unsubscribe(Graphics::onVerticalSyncChange);
        delete DEFAULT_MESH_SHADER;
        delete DEFAULT_MESH_MATERIAL;
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
        default_mesh_shader->setShaders(default_mesh_vertex_shader_code,
                                        default_mesh_fragment_shader_code, false);
        DEFAULT_MESH_SHADER = default_mesh_shader;
        Material *default_mesh_material = Material::create();
        default_mesh_material->setShader(DEFAULT_MESH_SHADER);
        DEFAULT_MESH_MATERIAL = default_mesh_material;
        //DEFAULT_INSTANCED_SHADER
        ShaderProgram *default_instanced_shader = ShaderProgram::create();
        default_instanced_shader->setShaders(default_instanced_vertex_shader_code,
                                             default_mesh_fragment_shader_code, false);
        DEFAULT_INSTANCED_SHADER = default_instanced_shader;
        //DEFAULT_SCREEN_SHADER
        ShaderProgram *default_screen_shader = ShaderProgram::create();
        default_screen_shader->setShaders(default_screen_vertex_shader_code,
                                          default_screen_fragment_shader_code, false);
        DEFAULT_SCREEN_SHADER = default_screen_shader;
        //DEFAULT_RENDER_TARGET
        render_target = new RenderTarget(900, 600, *DEFAULT_SCREEN_SHADER);
    }

    RenderTarget *Graphics::getRenderTarget() {
        return render_target;
    }

    GLFWwindow *Graphics::getWindow() {
        return window;
    }

    void Graphics::getWindowSize(int &width, int &height) {
        glfwGetWindowSize(window, &width, &height);
    }

    void Graphics::clear() {
        renderer->clear();
    }

    void Graphics::onWindowTitleChange(std::string title) {
        glfwSetWindowTitle(window, title.c_str());
    }

    void Graphics::clearDrawCache() {
        renderer->clearDrawCache();
    }

}








