#include "Camera.h"

namespace HBE
{
    void Camera::calculateProjection()
    {
        projection = glm::perspective<float>(glm::radians(fov), aspect_ratio, near, far);
        projection[1] = -projection[1];

        projection_dirty = false;
    }

    float Camera::getAspectRatio()
    {
        return aspect_ratio;
    }

    bool Camera::isProjectionDirty()
    {
        return projection_dirty;
    }

    void Camera::setFOV(float fov)
    {
        this->fov = fov;
        projection_dirty = true;
    }

    float Camera::getFOV()
    {
        return fov;
    }

    void Camera::setNearPlane(float near)
    {
        projection_dirty = true;
        this->near = near;
    }

    void Camera::setFarPlane(float far)
    {
        projection_dirty = true;
        this->far = far;
    }


    void Camera::calculateAspectRatio(vec2u res)
    {
        float new_aspect_ratio = static_cast<float>(res.x) / static_cast<float>(res.y);
        if (abs(new_aspect_ratio - aspect_ratio) <= glm::epsilon<float>())
        {
            projection_dirty = true;
        }
        aspect_ratio = new_aspect_ratio;
    }
}
