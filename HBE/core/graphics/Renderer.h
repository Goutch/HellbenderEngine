#pragma once
#include "Core.h"
#include "vector"
#include "glm/glm.hpp"

#include "Graphics.h"

using namespace glm;

namespace HBE {

    class RenderTarget;

    class Transform;

    class Mesh;

    class Framebuffer;

    class Material;

    class Window;

    class ResourceFactory;

    class Renderer {
    public:
        static Renderer *create();

        virtual ~Renderer() = default;

        virtual void render(const RenderTarget *render_target, const mat4 &projection_matrix,
                            const mat4 &view_matrix) = 0;

        virtual void present(const RenderTarget *render_target) = 0;

		virtual void beginFrame() = 0;
        virtual void endFrame() = 0;



        virtual void draw(const Transform &transform, const Mesh &mesh, const Material &material) = 0;

        virtual void drawInstanced(const Mesh &mesh, const Material &material) = 0;

        virtual void clear() const = 0;

        virtual const ResourceFactory *getResourceFactory() const = 0;
    };
}