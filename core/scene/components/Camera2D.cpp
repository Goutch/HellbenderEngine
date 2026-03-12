#include "Camera2D.h"
#include "glm/gtc/matrix_transform.hpp"

namespace HBE
{
    void Camera2D::setNearPlane(float near)
    {
        this->near = near;
        projection_dirty = true;
    }

    void Camera2D::setFarPlane(float far)
    {
        this->far = far;
        projection_dirty = true;
    }

    float Camera2D::getNearPlane()
    {
        return near;
    }

    float Camera2D::getFarPlane()
    {
        return far;
    }

    void Camera2D::calculateAspectRatio(vec2u res)
    {
        float new_aspect_ratio = static_cast<float>(res.x) / static_cast<float>(res.y);
        if (abs(new_aspect_ratio - aspect_ratio) <= glm::epsilon<float>())
        {
            projection_dirty = true;
        }
        aspect_ratio = new_aspect_ratio;
    }

    void Camera2D::calculateProjection()
    {
        if (zoom_ratio < 0.1)
            zoom_ratio = 0.1;
        projection = glm::ortho(zoom_ratio * -0.5f * aspect_ratio, zoom_ratio * 0.5f * aspect_ratio, zoom_ratio * 0.5f, zoom_ratio * -.5f, near, far);
        //projection[1] = -projection[1];
        projection_dirty = false;
    }

    bool Camera2D::isProjectionDirty() const
    {
        return projection_dirty;
    }

    float Camera2D::getZoomRatio()
    {
        return zoom_ratio;
    }

    void Camera2D::setZoomRatio(float ratio)
    {
        zoom_ratio = ratio;
        projection_dirty = true;
    }
}
