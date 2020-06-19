#pragma once


#include <array>
#include <core/math/Plane.h>
#include <core/entity/component/Component.h>

class Camera : public Component{
private:

    mat4 projection_matrix;
    float fov=80;
    std::array<Plane,6> frustum_planes;
    float aspect_ratio=1;
    float render_distance=500;
    void generateProjectionMatrix();
public:
    static const Camera* main;
    void onAttach() override;

    void setAspectRatio(float width, float height);
    void setAspectRatio(float aspect_ratio);
    float getAspectRatio();
    void setRenderDistance(float render_distance);
    float getRenderDistance();

    const mat4 &getProjectionMatrix() const;
    mat4 getViewMatrix() const;

    float getFOV() const;
    void setFOV(float fov);

    void calculateFrustumPlanes();
    bool isPointInFrustum(const vec3& point) const;
    bool isBoxInFrustum(const vec3& position,float size_x,float size_y,float size_z) const;
    bool isSphereInFrustum(const vec3 &position, float radius) const;
};