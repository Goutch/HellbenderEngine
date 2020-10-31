//
// Created by User on 2020-01-26.
//

#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"
#include <core/entity/Entity.h>
#include <core/graphics/Graphics.h>
#include <core/graphics/RenderTarget.h>
Camera* Camera::main= nullptr;
void Camera::onAttach() {
    Component::onAttach();
    if(Camera::main== nullptr)
    {
        Camera::main=this;
    }
    setAspectRatio(Graphics::getRenderTarget()->getWidth(),Graphics::getRenderTarget()->getHeight());

    Graphics::getRenderTarget()->onSizeChange.subscribe<Camera>(this,&Camera::onRenderTargetSizeChange);

    generateProjectionMatrix();
}

void Camera::onDetach() {
    Graphics::getRenderTarget()->onSizeChange.unsubsribe(this);
}

void Camera::onRenderTargetSizeChange(int width,int height)
{
    setAspectRatio(width,height);
    generateProjectionMatrix();
}

void Camera::generateProjectionMatrix() {
    if(render_mode==RenderMode::PERSPECTIVE)
    {
        projection_matrix = glm::perspective<float>(glm::radians(fov), aspect_ratio, 0.1f, render_distance);
    }
    if(render_mode==RenderMode::ORTHOGRAPHIC)
    {
        projection_matrix=glm::ortho(-0.5*aspect_ratio,0.5*aspect_ratio,-0.5,0.5,-1.0,1.0);
    }
    calculateFrustumPlanes();
}

float Camera::getAspectRatio() {
    return aspect_ratio;
}

float Camera::getRenderDistance() {
    return render_distance;
}

void Camera::setRenderDistance(float render_distance) {
    this->render_distance = render_distance;
    generateProjectionMatrix();
}

const mat4 &Camera::getProjectionMatrix() const {
    return projection_matrix;
}

void Camera::setAspectRatio(float width, float height) {
    aspect_ratio = width / height;
    generateProjectionMatrix();
}

void Camera::setAspectRatio(float aspect_ratio) {
    this->aspect_ratio = aspect_ratio;
    generateProjectionMatrix();
}

mat4 Camera::getViewMatrix() const {
    return glm::inverse(entity->getMatrix());
}

float Camera::getFOV() const {
    return fov;
}

void Camera::setFOV(float fov) {
    this->fov = fov;
    generateProjectionMatrix();
}

void Camera::calculateFrustumPlanes() {
    mat4 m = projection_matrix * getViewMatrix();
    frustum_planes[0].set((vec4(m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0], m[3][3] + m[3][0])));//left
    frustum_planes[1].set((vec4(m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0], m[3][3] - m[3][0])));//right
    frustum_planes[3].set((vec4(m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1], m[3][3] + m[3][1])));//bottom
    frustum_planes[2].set((vec4(m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1], m[3][3] - m[3][1])));//top
    frustum_planes[4].set((vec4(m[0][3] + m[0][2], m[1][3] + m[1][2], m[2][3] + m[2][2], m[3][3] + m[3][2])));//near
    frustum_planes[5].set((vec4(m[0][3] - m[0][2], m[1][3] - m[1][2], m[2][3] - m[2][2], m[3][3] - m[3][2])));//far
}

bool Camera::isPointInFrustum(const vec3 &point) const {
    for (int i = 0; i < frustum_planes.size(); i++) {
        if (0 > frustum_planes[i].distance(point)) {
            return false;
        }
    }
    return true;
}

bool Camera::isSphereInFrustum(const vec3 &position, float radius) const {
    for (int i = 0; i < frustum_planes.size(); i++) {
        if (0 > frustum_planes[i].distance(position) + radius) {
            return false;
        }
    }
    return true;
}

bool Camera::isBoxInFrustum(const vec3 &position, float size_x, float size_y, float size_z) const {

    if (isPointInFrustum(position)) { return true; }
    if (isPointInFrustum(position + vec3(size_x, size_y, size_z))) { return true; }
    if (isPointInFrustum(position + vec3(size_x, 0.0f, 0.0f))) { return true; }
    if (isPointInFrustum(position + vec3(0.0f, size_y, 0.0f))) { return true; }
    if (isPointInFrustum(position + vec3(0.0f, 0.0f, size_z))) { return true; }
    if (isPointInFrustum(position + vec3(size_x, size_y, 0.0f))) { return true; }
    if (isPointInFrustum(position + vec3(0.0f, size_y, size_z))) { return true; }
    return isPointInFrustum(position + vec3(size_x, 0.0f, size_z));
}

void Camera::setRenderMode(RenderMode mode) {
    render_mode=mode;
    generateProjectionMatrix();
}















