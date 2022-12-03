#pragma once

#include <core/resource/Texture.h>
#include "Core.h"
#include "vector"
#include "glm/glm.hpp"


namespace HBE {

	class RenderTarget;

	struct Transform;

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

	struct PushConstantInfo {
		std::string name;
		void *data;
		uint32_t size;
	};

	struct DrawCmdInfo {
		const Mesh *mesh;
		GraphicPipelineInstance *pipeline_instance;
		uint32_t push_constants_count = 0;
		PushConstantInfo *push_constants = nullptr;
		DRAW_CMD_FLAGS flags = DRAW_CMD_FLAG_NONE;
	};

	class Renderer {
	public:
		static Renderer *create();

		virtual ~Renderer() = default;

		virtual void render(const RenderTarget *render_target,
							const mat4 &projection_matrix,
							const mat4 &view_matrix) = 0;

		virtual void raytrace(const RootAccelerationStructure &root_acceleration_structure, RaytracingPipelineInstance &pipeline, const mat4 &projection_matrix, const mat4 &view_matrix, const vec2i resolution) = 0;

		virtual void beginFrame() = 0;

		virtual void present(const Texture *image) = 0;

		virtual void endFrame() = 0;

		virtual void draw(DrawCmdInfo &draw_cmd_info) = 0;

		virtual RenderTarget *getDefaultRenderTarget() = 0;

		virtual const ResourceFactory *getResourceFactory() const = 0;

		virtual void createDefaultResources() = 0;

		virtual uint32_t getFrameCount() const = 0;

		virtual uint32_t getCurrentFrame() const = 0;
	};
}