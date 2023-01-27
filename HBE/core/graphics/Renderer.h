#pragma once

#include "Core.h"
#include "vector"
#include "glm/glm.hpp"
#include <string>

namespace HBE {

	class RenderTarget;

	class Texture;

	class Mesh;

	class Framebuffer;

	class Window;

	class ResourceFactory;

	class GraphicPipeline;

	class GraphicPipelineInstance;

	class RootAccelerationStructure;

	class RaytracingPipelineInstance;

	typedef uint32_t DRAW_CMD_FLAGS;
	enum DRAW_CMD_FLAG {
		DRAW_CMD_FLAG_NONE = 0,
		DRAW_CMD_FLAG_ORDERED = 1 << 0,
	};
	typedef uint32_t RENDER_CMD_FLAGS;
	enum RENDER_CMD_FLAG {
		REDNER_CMD_FLAG_NONE = 0,
		RENDER_CMD_FLAG_INVERSE_Y = 1 << 0,
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
		void *data;
		uint32_t size;
	};

	struct DrawCmdInfo {
		const Mesh *mesh= nullptr;
		GraphicPipelineInstance *pipeline_instance= nullptr;
		uint32_t layer = 0;
		uint32_t push_constants_count = 0;
		PushConstantInfo *push_constants = nullptr;
		DRAW_CMD_FLAGS flags = DRAW_CMD_FLAG_NONE;
	};

	struct RenderCmdInfo {
		const RenderTarget *render_target;
		const uint32_t layer_mask;
		RENDER_CMD_FLAGS flags;
		mat4 view;
		mat4 projection;
	};

	struct TraceRaysCmdInfo {
		const RootAccelerationStructure *root_acceleration_structure;
		RaytracingPipelineInstance *pipeline_instance;
		vec2i resolution;
		TRACE_RAYS_CMD_FLAGS flags;
	};

	struct PresentCmdInfo {
		const Texture **images;
		uint32_t image_count;
		PRESENT_CMD_FLAGS flags;
	};

	class Renderer {
	public:
		static Renderer *create();

		virtual ~Renderer() = default;

		virtual void draw(DrawCmdInfo &draw_cmd_info) = 0;

		virtual void render(RenderCmdInfo &render_cmd_info) = 0;

		virtual void traceRays(TraceRaysCmdInfo &trace_rays_cmd_info) = 0;

		virtual void present(PresentCmdInfo &present_cmd_info) = 0;

		virtual void beginFrame() = 0;

		virtual void endFrame() = 0;

		virtual RenderTarget *getDefaultRenderTarget() = 0;

		virtual RenderTarget *getUIRenderTarget() = 0;

		virtual const ResourceFactory *getResourceFactory() const = 0;

		virtual void createDefaultResources() = 0;

		virtual uint32_t getFrameCount() const = 0;

		virtual uint32_t getCurrentFrame() const = 0;


	};
}