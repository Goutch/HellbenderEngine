
#include "Graphics.h"
#include <core/utility/Log.h>
#include <core/utility/Event.h>
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
	Renderer *Graphics::renderer = nullptr;
	Window *Graphics::window = nullptr;

	void Graphics::init() {
		window = Window::create(1920, 1080);
		renderer = Renderer::create();
		Resources::init(*renderer->getResourceFactory());
		renderer->createDefaultResources();
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


}







