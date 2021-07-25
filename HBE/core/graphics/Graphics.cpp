
#include "Graphics.h"

#include "Framebuffer.h"
#include <core/entity/component/Transform.h>
#include <core/utility/Log.h>
#include <core/utility/Event.h>
#include <core/utility/Geometry.h>
#include <core/graphics/RenderTarget.h>
#include "core/resource/GraphicPipeline.h"
#include <Configs.h>
#include <core/resource/Model.h>
#include "Renderer.h"
#include "core/resource/Material.h"
#include "Window.h"
#include "core/resource/Resources.h"
#include "core/resource/VertexLayout.h"

namespace HBE {
	const Mesh *Graphics::DEFAULT_CUBE = nullptr;
	const Mesh *Graphics::DEFAULT_QUAD = nullptr;
	const GraphicPipeline *Graphics::DEFAULT_MESH_PIPELINE = nullptr;
	const GraphicPipeline *Graphics::DEFAULT_SCREEN_PIPELINE = nullptr;
	const GraphicPipeline *Graphics::DEFAULT_INSTANCED_PIPELINE = nullptr;
	const Material *Graphics::DEFAULT_MESH_MATERIAL = nullptr;
	DRAW_FLAGS Graphics::default_draw_flags;
	Renderer *Graphics::renderer = nullptr;
	Window *Graphics::window = nullptr;
	RenderTarget *Graphics::render_target = nullptr;

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
		window->onWindowSizeChange.subscribe(&Graphics::onWindowSizeChange);
		initializeDefaultVariables();
	}

	void Graphics::onWindowSizeChange(int width, int height) {
		if (!Configs::isPresentAutomatic() && render_target != nullptr) {
			render_target->setSize(width, height);
		}
	}

	void Graphics::draw(const Transform &transform, const Mesh &mesh, const Material &material) {
		renderer->draw(transform, mesh, material);
	}

	void Graphics::drawInstanced(const Mesh &mesh, const Material &material) {
		renderer->drawInstanced(mesh, material);
	}

	void Graphics::render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix) {
		//render_target->getFramebuffer().bind();
		renderer->clear();
		renderer->render(render_target, projection_matrix, view_matrix);
		//render_target->getFramebuffer().unbind();

	}

	void Graphics::present(const RenderTarget *render_target) {
		renderer->clear();
		renderer->present(render_target);
	}

	void Graphics::terminate() {

		delete DEFAULT_MESH_PIPELINE;
		delete DEFAULT_MESH_MATERIAL;
		delete DEFAULT_SCREEN_PIPELINE;
		delete DEFAULT_INSTANCED_PIPELINE;
		delete DEFAULT_QUAD;
		delete DEFAULT_CUBE;
		delete render_target;
		delete renderer;
		delete window;
	}

	void Graphics::initializeDefaultVariables() {
		//-----------------------------------DEFAULT_CUBE---------------------------------
		/*  Mesh *cube = new Mesh();
		 Geometry::createCube(*cube, 1, 1, 1);
		 DEFAULT_CUBE = cube;
		 //-----------------------------------DEFAULT_QUAD---------------------------------
		 Mesh *quad = new Mesh();
		 Geometry::createQuad(*quad, 1, 1);
		 DEFAULT_QUAD = quad;

		 //-----------------------------------DEFAULT_MESH_MATERIAL--------------------------
		 ShaderDB::add("default/vert_unlit", default_mesh_vertex_shader_code, SHADER_TYPE::VERTEX);
		 ShaderDB::add("default/frag_unlit", default_mesh_fragment_shader_code, SHADER_TYPE::FRAGMENT);
		 GraphicPipeline *default_graphic_pipeline = new GraphicPipeline();
		 default_graphic_pipeline->setShaders(
				 ShaderDB::get("default/vert_unlit"),
				 ShaderDB::get("default/frag_unlit"));

		 DEFAULT_MESH_PIPELINE = default_graphic_pipeline;
		 Material *default_mesh_material = new Material();
		 default_mesh_material->setPipeline(DEFAULT_MESH_PIPELINE);
		 DEFAULT_MESH_MATERIAL = default_mesh_material;

		 //-----------------------------------DEFAULT_INSTANCED_PIPELINE----------------
		 ShaderDB::add("default/vert_unlit_instanced", default_instanced_vertex_shader_code, SHADER_TYPE::VERTEX);
		 GraphicPipeline *default_instanced_shader_program = new GraphicPipeline();
		 default_instanced_shader_program->setShaders(
				 ShaderDB::get("default/vert_unlit_instanced"),
				 ShaderDB::get("default/frag_unlit"));
		 DEFAULT_INSTANCED_PIPELINE = default_instanced_shader_program;
*/
		//------------------------------------DEFAULT_SCREEN_PIPELINE---------------------------
		/*auto screen_vertex = Resources::createInRegistry<Shader>("default/screen_vertex");
		auto screen_frag = Resources::createInRegistry<Shader>("default/screen_frag");
		screen_vertex->setSource(default_screen_vertex_shader_code, SHADER_TYPE::VERTEX);
		screen_frag->setSource(default_screen_fragment_shader_code, SHADER_TYPE::FRAGMENT);
		auto default_screen_pipeline = Resources::createInRegistry<GraphicPipeline>("default/screen_pipeline");
		auto default_screen_layout = Resources::createInRegistry<VertexLayout>("default/screen_layout");
		default_screen_layout->setLayoutTypes({GLSL_TYPE::VEC2F, GLSL_TYPE::VEC2F});
		default_screen_pipeline->setShaders(
				screen_vertex,
				screen_frag, default_screen_layout);*/
		//------------------------------------DEFAULT_RENDER_TARGET------------------------------
		int width, height;
		window->getSize(width, height);
		render_target = new RenderTarget(width, height, *DEFAULT_SCREEN_PIPELINE);

		default_draw_flags = DRAW_FLAGS_NONE;
	}

	RenderTarget *Graphics::getRenderTarget() {
		return render_target;
	}

	Window *Graphics::getWindow() {
		return window;
	}

	void Graphics::clear() {
		renderer->clear();
	}

	void Graphics::endFrame() {
		renderer->endFrame();
	}

	void Graphics::setDefaultDrawFlags(DRAW_FLAGS draw_flags) {
		default_draw_flags = draw_flags;
	}

	Renderer *Graphics::getRenderer() {
		return renderer;
	}

	DRAW_FLAGS Graphics::getDefaultDrawFlags() {
		return default_draw_flags;
	}

	void Graphics::beginFrame() {
		renderer->beginFrame();
	}

}








