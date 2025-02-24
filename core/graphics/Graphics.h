#pragma once

#include "Core.h"
#include <vector>
#include <string>
#include "glm/glm.hpp"
#include "Renderer.h"
#include "GraphicLimits.h"

namespace HBE {
	class Renderer;

	class RasterizationTarget;

	class RasterizationPipeline;

	class Mesh;

	struct Transform;

	class Window;

	class RasterizationPipelineInstance;

	class RaytracingPipelineInstance;

	class RootAccelerationStructure;

	class RasterizationTarget;

	class Image;

	using namespace utils;

	class HB_API Graphics {
		static Renderer *renderer;
		static Window *window;
		static GraphicLimits limits;
	public:
		static Event<uint32_t> onFrameChange;
		static Event<uint32_t, uint32_t> onSwapchainResized;

		static const Mesh *DEFAULT_QUAD;
		static const Mesh *DEFAULT_CUBE;

		static void init(const char *title, uint32_t width, uint32_t height, bool fullscreen = false);

		static Window *getWindow();

		static const GraphicLimits &getLimits();

		static void rasterize(RasterizeCmdInfo &rasterize_cmd_info);

		static void traceRays(TraceRaysCmdInfo &trace_rays_cmd_info);

		static void present(PresentCmdInfo &present_cmd_info);

		static void computeDispatch(ComputeDispatchCmdInfo &compute_dispatch_cmd_info);

		static RasterizationTarget *getDefaultRenderTarget();

		static void terminate();

		static void waitCurrentFrame();

		static void beginFrame();

		static void endFrame();

		static Renderer *getRenderer();

		static uint32_t getCurrentFrame();

		static uint32_t getFrameCount();

		static void waitLastFrame();
	};
}