#pragma once

#include "Core.h"
#include "core/Graphics.h"
namespace HBE
{
    class Scene;

    class HB_API System
    {
    protected:
        Context* context;
        Scene* scene;

    public:
        virtual ~System() = default;

        System(Scene* scene);
    };
}
