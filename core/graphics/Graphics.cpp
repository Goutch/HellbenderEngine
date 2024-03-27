
#include "Graphics.h"
#include <core/utility/Log.h>
#include <core/resource/RenderTarget.h>
#include <Configs.h>
#include <core/resource/Model.h>
#include "Renderer.h"
#include "Window.h"
#include "core/resource/Resources.h"

namespace HBE {
	const Mesh *Graphics::DEFAULT_CUBE = nullptr;
	const Mesh *Graphics::DEFAULT_QUAD = nullptr;
	Event<uint32_t> Graphics::onFrameChange;
	Event<uint32_t, uint32_t> Graphics::onSwapchainResized;
	Renderer *Graphics::renderer = nullptr;
	Window *Graphics::window = nullptr;
	GraphicLimits Graphics::limits;
	void Graphics::init(const char *title, uint32_t width, uint32_t height, bool fullscreen) {

		window = Window::create(title, width, height);
		if (fullscreen)
			window->setFullscreen(fullscreen);
		renderer = Renderer::create();
		Resources::init(*renderer->getResourceFactory());
		renderer->createDefaultResources();
		limits = renderer->getLimits();
	}

	void Graphics::render(RenderCmdInfo &render_cmd_info) {
		renderer->render(render_cmd_info);
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
		onFrameChange.invoke(renderer->getCurrentFrame());
	}

	Renderer *Graphics::getRenderer() {
		return renderer;
	}

	void Graphics::beginFrame() {
		renderer->beginFrame();
	}

	void Graphics::traceRays(TraceRaysCmdInfo &trace_rays_cmd_info) {
		renderer->traceRays(trace_rays_cmd_info);
	}


	uint32_t Graphics::getCurrentFrame() {
		return renderer->getCurrentFrame();
	}

	uint32_t Graphics::getFrameCount() {
		return renderer->getFrameCount();
	}

	void Graphics::present(PresentCmdInfo &present_cmd_info) {
		renderer->present(present_cmd_info);
	}

	void Graphics::waitCurrentFrame() {
		renderer->waitCurrentFrame();
	}

	void Graphics::waitLastFrame() {
		renderer->waitLastFrame();
	}

	const GraphicLimits &Graphics::getLimits() {
		return limits;
	}

}








