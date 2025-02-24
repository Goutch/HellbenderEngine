#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "vector"
#include "glm/glm.hpp"
#include "Event.h"
#include <string>
#include "GraphicLimits.h"

namespace HBE {

	class RasterizationTarget;

	class Image;

	class Mesh;

	class Framebuffer;

	class Window;

	class ResourceFactory;

	class RasterizationPipeline;

	class RasterizationPipelineInstance;

	class RootAccelerationStructure;

	class RaytracingPipelineInstance;

	class RenderGraph;

	class ComputeInstance;

	typedef uint32_t DRAW_CMD_FLAGS;
	enum DRAW_CMD_FLAG {
		DRAW_CMD_FLAG_NONE = 0,
		DRAW_CMD_FLAG_ORDERED = 1 << 0,
	};
	typedef uint32_t RASTERIZE_CMD_FLAGS;
	enum RASTERIZE_CMD_FLAG {
		RASTERIZE_CMD_FLAG_NONE = 0,
		RASTERIZE_CMD_FLAG_INVERSE_Y = 1 << 0,
	};
	typedef uint32_t TRACE_RAYS_CMD_FLAGS;
	enum TRACE_RAYS_CMD_FLAG {
		TRACE_RAYS_CMD_FLAG_NONE = 0,
	};

	typedef uint32_t PRESENT_CMD_FLAGS;
	enum PRESENT_CMD_FLAG {
		PRESENT_CMD_FLAG_NONE = 0,
	};

	struct PushConstantInfo {
		std::string name;
		//array of 128bytes max
		void *data;
		uint32_t size;
	};

	struct DrawCmdInfo {
		const Mesh *mesh = nullptr;
		RasterizationPipelineInstance *pipeline_instance = nullptr;
		uint32_t layer = 0;
		uint32_t order_in_layer = 0;//ignored if ordered flag is not set
		uint32_t push_constants_count = 0;
		PushConstantInfo *push_constants = nullptr;
		DRAW_CMD_FLAGS flags = DRAW_CMD_FLAG_NONE;
	};

	struct RasterizeCmdInfo {
		const RasterizationTarget *render_target;
		RenderGraph *render_graph;
		uint32_t layer_mask;
		mat4 view;
		mat4 projection;
		RASTERIZE_CMD_FLAGS flags;
	};

	struct TraceRaysCmdInfo {
		RaytracingPipelineInstance *pipeline_instance;
		vec2i resolution;
		TRACE_RAYS_CMD_FLAGS flags;
	};

	struct PresentCmdInfo {
		Image **images;
		uint32_t image_count;
		PRESENT_CMD_FLAGS flags;
	};

	struct ComputeDispatchCmdInfo {
		ComputeInstance *pipeline_instance;
		uint32_t size_x = 1;
		uint32_t size_y = 1;
		uint32_t size_z = 1;
	};

	class Renderer {
	public:
		static Renderer *create();

		virtual ~Renderer() = default;

		virtual void rasterize(RasterizeCmdInfo &rasterize_cmd_info) = 0;

		virtual void traceRays(TraceRaysCmdInfo &trace_rays_cmd_info) = 0;

		virtual void present(PresentCmdInfo &present_cmd_info) = 0;

		virtual void computeDispatch(ComputeDispatchCmdInfo &compute_dispatch_cmd_info) = 0;

		virtual void beginFrame() = 0;

		virtual void endFrame() = 0;

		virtual RasterizationTarget *getDefaultRenderTarget() = 0;

		virtual RasterizationTarget *getUIRenderTarget() = 0;

		virtual const ResourceFactory *getResourceFactory() const = 0;

		virtual void createDefaultResources() = 0;

		virtual uint32_t getFrameCount() const = 0;

		virtual uint32_t getCurrentFrame() const = 0;

		virtual void waitCurrentFrame() = 0;

		virtual void waitLastFrame() = 0;

		virtual GraphicLimits getLimits() = 0;
	};
}