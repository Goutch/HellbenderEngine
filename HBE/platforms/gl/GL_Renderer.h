#pragma once

class ShaderProgram;

class Transform;

class Mesh;

#include <core/graphics/Renderer.h>
#include "list"

class GL_Renderer : public Renderer {
private:
    struct RenderObject {
        const Transform *transform;
        const Mesh *mesh;
        const Material *material;
    };
    std::list<RenderObject> render_objects;

private:
    GLFWwindow *window;
public:
    void clear() const override;

    void init() override;

    void draw(const Transform &transform, const Mesh &mesh, const Material &material) override;

    void drawInstanced(const Mesh &mesh, const Material &material) override;

    void render(const RenderTarget *render_target, const mat4 &projection_matrix, const mat4 &view_matrix) override;

    void present(const RenderTarget *render_target) override;

    void clearDrawCache() override;

    GLFWwindow *createWindow(int width, int height) override;
};
