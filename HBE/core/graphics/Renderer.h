#pragma once

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

	class Renderer {
	public:
		static Renderer *create();

		virtual ~Renderer() = default;

		virtual void render(const RenderTarget *render_target, const mat4 &projection_matrix,
							const mat4 &view_matrix) = 0;

		virtual void beginFrame() = 0;

		virtual void endFrame(bool present) = 0;

		virtual void setCurrentRenderTarget(RenderTarget *renderTarget) = 0;

		virtual void draw(mat4 transform_matrix, const Mesh &mesh, GraphicPipeline &pipeline) = 0;

		virtual void drawInstanced(const Mesh &mesh, GraphicPipeline &pipeline) = 0;

		virtual RenderTarget *getDefaultRenderTarget() = 0;

		virtual const ResourceFactory *getResourceFactory() const = 0;

		virtual void createDefaultResources() =0;
	};
}