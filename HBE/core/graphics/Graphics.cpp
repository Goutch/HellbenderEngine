
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

#elif RENDERER == VULKAN_RENDERER
#include "platforms/vk/VK_Renderer.h"
#endif

#include "core/resource/Material.h"

namespace HBE {
    const Mesh *Graphics::DEFAULT_CUBE = nullptr;
    const Mesh *Graphics::DEFAULT_QUAD = nullptr;
    const ShaderProgram *Graphics::DEFAULT_MESH_SHADER_PROGRAM = nullptr;
    const ShaderProgram *Graphics::DEFAULT_SCREEN_SHADER_PROGRAM = nullptr;
    const ShaderProgram *Graphics::DEFAULT_INSTANCED_SHADER_PROGRAM = nullptr;
    const Material *Graphics::DEFAULT_MESH_MATERIAL = nullptr;
    DRAW_FLAGS Graphics::default_draw_Flags;
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

    void Graphics::onWindowSizeChangeCallback(GLFWwindow *window, int32 width, int32 height) {
        if (!Configs::isCustonRenderingOn()) {
            render_target->setSize(width, height);
        }
        onWindowSizeChange.invoke(width, height);
    }

    void Graphics::draw(const Transform &transform, const Mesh &mesh, const Material &material, DRAW_FLAGS draw_flags) {
        renderer->draw(transform, mesh, material,draw_flags);
    }

    void Graphics::drawInstanced(const Mesh &mesh, const Material &material, DRAW_FLAGS draw_flags) {
        renderer->drawInstanced(mesh, material,draw_flags);
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
        delete DEFAULT_MESH_SHADER_PROGRAM;
        delete DEFAULT_MESH_MATERIAL;
        delete DEFAULT_SCREEN_SHADER_PROGRAM;
        delete DEFAULT_INSTANCED_SHADER_PROGRAM;
        delete DEFAULT_QUAD;
        delete DEFAULT_CUBE;
        delete render_target;
        delete renderer;

    }

    void Graphics::initializeDefaultVariables() {
        //-----------------------------------DEFAULT_CUBE---------------------------------
        Mesh *cube = Mesh::create();
        Geometry::createCube(*cube, 1, 1, 1);
        DEFAULT_CUBE = cube;
        //-----------------------------------DEFAULT_QUAD---------------------------------
        Mesh *quad = Mesh::create();
        Geometry::createQuad(*quad, 1, 1);
        DEFAULT_QUAD = quad;

        //-----------------------------------DEFAULT_MESH_MATERIAL--------------------------
        Shader* default_mesh_vertex_shader=Shader::create(SHADER_TYPE::VERTEX,default_mesh_vertex_shader_code);
        Shader* default_mesh_fragment_shader=Shader::create(SHADER_TYPE::FRAGMENT,default_mesh_fragment_shader_code);
        ShaderProgram *default_mesh_shader_program = ShaderProgram::create();
        default_mesh_shader_program->setShaders({{SHADER_TYPE::VERTEX,default_mesh_vertex_shader},{SHADER_TYPE::FRAGMENT,default_mesh_fragment_shader}});
        delete default_mesh_vertex_shader;
        delete default_mesh_fragment_shader;
        DEFAULT_MESH_SHADER_PROGRAM = default_mesh_shader_program;
        Material *default_mesh_material = Material::create();
        default_mesh_material->setShader(DEFAULT_MESH_SHADER_PROGRAM);
        DEFAULT_MESH_MATERIAL = default_mesh_material;

        //-----------------------------------DEFAULT_INSTANCED_SHADER_PROGRAM----------------
        Shader* default_instanced_vertex_shader=Shader::create(SHADER_TYPE::VERTEX,default_instanced_vertex_shader_code);
        Shader* default_instanced_fragment_shader=Shader::create(SHADER_TYPE::FRAGMENT,default_mesh_fragment_shader_code);
        ShaderProgram *default_instanced_shader_program = ShaderProgram::create();
        default_instanced_shader_program->setShaders({{SHADER_TYPE::VERTEX,default_instanced_vertex_shader},{SHADER_TYPE::FRAGMENT,default_instanced_fragment_shader}});
        delete default_instanced_vertex_shader;
        delete default_instanced_fragment_shader;
        DEFAULT_INSTANCED_SHADER_PROGRAM = default_instanced_shader_program;

        //------------------------------------DEFAULT_SCREEN_SHADER_PROGRAM---------------------------
        Shader* default_screen_vertex_shader=Shader::create(SHADER_TYPE::VERTEX,default_screen_vertex_shader_code);
        Shader* default_screen_fragment_shader=Shader::create(SHADER_TYPE::FRAGMENT,default_screen_fragment_shader_code);
        ShaderProgram *default_screen_shader_program = ShaderProgram::create();
        default_screen_shader_program->setShaders({{SHADER_TYPE::VERTEX,default_screen_vertex_shader},{SHADER_TYPE::FRAGMENT,default_screen_fragment_shader}});
        delete default_screen_vertex_shader;
        delete default_screen_fragment_shader;
        DEFAULT_SCREEN_SHADER_PROGRAM = default_screen_shader_program;
        //------------------------------------DEFAULT_RENDER_TARGET------------------------------
        int32 width,height;
        Graphics::getWindowSize(width,height);
        render_target = new RenderTarget(width, height, *DEFAULT_SCREEN_SHADER_PROGRAM);

        default_draw_Flags=DRAW_FLAGS_NONE;

        Log::message("init finish");
    }

    RenderTarget *Graphics::getRenderTarget() {
        return render_target;
    }

    GLFWwindow *Graphics::getWindow() {
        return window;
    }

    void Graphics::getWindowSize(int32 &width, int32 &height) {
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

    void Graphics::setDefaultDrawFlags(DRAW_FLAGS draw_flags) {
        default_draw_Flags=draw_flags;
    }

}








