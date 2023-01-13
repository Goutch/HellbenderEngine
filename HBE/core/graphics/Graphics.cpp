
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

	void Graphics::init() {
		window = Window::create(1920, 1080);
		renderer = Renderer::create();
		Resources::init(*renderer->getResourceFactory());
		renderer->createDefaultResources();
	}

	void Graphics::draw(DrawCmdInfo &DrawCmdInfo) {
		renderer->draw(DrawCmdInfo);
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

	void Graphics::endFrame() {
		renderer->endFrame();
	}

	Renderer *Graphics::getRenderer() {
		return renderer;
	}

	void Graphics::beginFrame() {
		renderer->beginFrame();
	}

	void Graphics::raytrace(const RootAccelerationStructure &root_acceleration_structure,
							RaytracingPipelineInstance &pipeline,
							const mat4 &projection_matrix,
							const mat4 &view_matrix,
							vec2u resolution) {
		renderer->raytrace(root_acceleration_structure, pipeline, projection_matrix, view_matrix, resolution);
	}


	uint32_t Graphics::getCurrentFrame() {
		return renderer->getCurrentFrame();
	}

	uint32_t Graphics::getFrameCount() {
		return renderer->getFrameCount();
	}

	void Graphics::present(const Texture *texture) {
		renderer->present(texture);
	}
}








