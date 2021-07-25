#pragma once

#include <core/graphics/Renderer.h>
#include "unordered_map"
#include "list"

namespace HBE {
    class GL_Window;

#define MAP_LIST(T1, T2) std::unordered_map<T1,T2>

    class GraphicPipeline;

    class Transform;

    class GL_ResourceFactory;

    class Mesh;

    class GL_Renderer : public Renderer {
    private:
        struct RenderObject {
            const Transform *transform;
            const Mesh *mesh;
            const Material *material;
        };
        MAP_LIST(const Material*,MAP_LIST(const Mesh*, std::list<const Transform*>)) render_cache;
    private:
        GL_Window *window;
        GL_ResourceFactory *factory;
    public:

        GL_Renderer();

        virtual ~GL_Renderer();

        void clear() const override;

        void
        draw(const Transform &transform, const Mesh &mesh, const Material &material) override;

        void drawInstanced(const Mesh &mesh, const Material &material) override;

        void render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix) override;

        void present(const RenderTarget *render_target) override;

        void clearDrawCache() override;

        const IResourceFactory *getResourceFactory() const override;
    };

}