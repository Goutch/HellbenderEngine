#pragma once

#include "Core.h"
#include <vector>
#include <string>
#include "glm/glm.hpp"
#include "Renderer.h"


namespace HBE {
	class Renderer;

	class RenderTarget;

	class GraphicPipeline;

	class Mesh;

	struct Transform;

	class Window;

	class GraphicPipelineInstance;

	template<typename... Args>
	class Event;

	class RaytracingPipelineInstance;

	class RootAccelerationStructure;

	class RenderTarget;

	class Texture;

	class HB_API Graphics {
		static Renderer *renderer;
		static Window *window;

	public:
        static Event<uint32_t> onFrameChange;

		static const Mesh *DEFAULT_QUAD;
		static const Mesh *DEFAULT_CUBE;

		static void init();

		static Window *getWindow();

		static void render(RenderCmdInfo &render_cmd_info);

		static void traceRays(TraceRaysCmdInfo &trace_rays_cmd_info);

		static void present(PresentCmdInfo &present_cmd_info);

		static RenderTarget *getDefaultRenderTarget();

		static void terminate();

		static void waitCurrentFrame();

		static void beginFrame();

		static void endFrame();

		static Renderer *getRenderer();

		static uint32_t getCurrentFrame();

		static uint32_t getFrameCount();
	};
}