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

	class Renderer {
	public:
		static Renderer *create();

		virtual ~Renderer() = default;

		virtual void render(const RenderTarget *render_target,
							const mat4 &projection_matrix,
							const mat4 &view_matrix) = 0;

		virtual void raytrace(const RootAccelerationStructure &root_acceleration_structure, RaytracingPipelineInstance &pipeline, const mat4 &projection_matrix, const mat4 &view_matrix, const vec2i resolution) = 0;

		virtual void beginFrame() = 0;

		virtual void present(HBE::Texture *image) = 0;

		virtual void endFrame() = 0;

		virtual void draw(mat4 transform_matrix, const Mesh &mesh, GraphicPipelineInstance &material) = 0;

		virtual void drawInstanced(const Mesh &mesh, GraphicPipelineInstance &material) = 0;

		virtual RenderTarget *getDefaultRenderTarget() = 0;

		virtual const ResourceFactory *getResourceFactory() const = 0;

		virtual void createDefaultResources() = 0;

		virtual uint32_t getFrameCount() const = 0;

		virtual uint32_t getCurrentFrame() const = 0;
	};
}