#pragma once


#include <array>
#include <core/math/Plane.h>
#include <core/entity/component/Component.h>
#include <glm/glm.hpp>
#include "Core.h"

class RenderTarget;

enum RenderMode {
    ORTHOGRAPHIC,
    PERSPECTIVE,
};

class HB_API Camera : public Component {
public:
    static Camera *main;
private:
    float fov = 80;
    float aspect_ratio = 1;
    float render_distance = 500;
    RenderMode render_mode = PERSPECTIVE;
    RenderTarget *render_target = nullptr;

    mat4 projection_matrix = mat4(1.0f);
    std::array<Plane, 6> frustum_planes;

public:

    std::string toString() const override;

    void onAttach() override;

    ~Camera() override;

    void setAspectRatio(float width, float height);

    void setAspectRatio(float aspect_ratio);

    float getAspectRatio();

    void setRenderDistance(float render_distance);

    float getRenderDistance();

    const mat4 &getProjectionMatrix() const;

    mat4 getViewMatrix() const;

    float getFOV() const;

    void setFOV(float fov);

    void setRenderTarget(RenderTarget *render_target);

    const RenderTarget *getRenderTarget();

    void calculateFrustumPlanes();

    bool isPointInFrustum(const vec3 &point) const;

    bool isBoxInFrustum(const vec3 &position, float size_x, float size_y, float size_z) const;

    bool isSphereInFrustum(const vec3 &position, float radius) const;

    void setRenderMode(RenderMode mode);

    void onRenderTargetSizeChange(int width, int height);

    void serialize(Serializer *serializer) const override;

    void deserialize(Deserializer *deserializer) override;

private:
    void generateProjectionMatrix();
};