
#include "Graphics.h"
#include <core/utility/Log.h>
#include <core/utility/Event.h>
#include <HBE/core/resource/RenderTarget.h>
#include <Configs.h>
#include <core/resource/Model.h>
#include "Renderer.h"
#include "Window.h"
#include "core/resource/Resources.h"

namespace HBE {
	const Mesh *Graphics::DEFAULT_CUBE = nullptr;
	const Mesh *Graphics::DEFAULT_QUAD = nullptr;

	Renderer *Graphics::renderer = nullptr;
	Window *Graphics::window = nullptr;

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


	void Graphics::init() {
		window = Window::create(900, 600);
		renderer = Renderer::create();
		Resources::init(*renderer->getResourceFactory());
		renderer->createDefaultResources();
	}

	void Graphics::draw(mat4 transform_matrix, const Mesh &mesh, Material &material) {
		renderer->draw(transform_matrix, mesh, material);
	}

	void Graphics::drawInstanced(const Mesh &mesh, Material &material) {
		renderer->drawInstanced(mesh, material);
	}

	void Graphics::render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix) {
		renderer->render(render_target, projection_matrix, view_matrix);
	}

	void Graphics::terminate() {


		delete DEFAULT_QUAD;
		delete DEFAULT_CUBE;
		delete renderer;
		delete window;
	}

	RenderTarget *Graphics::getDefaultRenderTarget() {
		return renderer->getDefaultRenderTarget();
	}

	Window *Graphics::getWindow() {
		return window;
	}

	void Graphics::endFrame(bool present) {
		renderer->endFrame(present);
	}

	Renderer *Graphics::getRenderer() {
		return renderer;
	}

	void Graphics::beginFrame() {
		renderer->beginFrame();
	}

	void Graphics::setRenderTarget(RenderTarget *render_target) {
		renderer->setCurrentRenderTarget(render_target);
	}

}








