#include "PixelCamera.h"
#include "glm/gtc/matrix_transform.hpp"

namespace HBE
{
    void PixelCamera::calculateProjection()
    {
        projection = glm::ortho(0.0f,
                                static_cast<float>(resolution.x),
                                static_cast<float>(resolution.y),
                                0.0f,
                                near, far);
    }

    bool PixelCamera::isProjectionDirty() const
    {
        return projection_dirty;
    }


    void PixelCamera::setResolution(vec2u resolution)
    {
        if (resolution != this->resolution)
            projection_dirty = true;
        this->resolution = resolution;
    }

    vec2u PixelCamera::getResolution()
    {
        return resolution;
    }

    void PixelCamera::setNearPlane(float near)
    {
        this->near = near;
        projection_dirty = true;
    }

    void PixelCamera::setFarPlane(float far)
    {
        this->far = far;
        projection_dirty = true;
    }

    float PixelCamera::getNearPlane()
    {
        return near;
    }

    float PixelCamera::getFarPlane()
    {
        return far;
    }
}
