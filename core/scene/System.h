#pragma once

#include "Core.h"
#include "core/Graphics.h"
#include "core/input/Input.h"
#include "core/graphics/Window.h"
namespace HBE
{
    class Scene;

    class HB_API System
    {
    protected:
        Context& context;
		Input& input;
		Window& window;
        Scene* scene;

    public:
        virtual ~System() = default;

        System(Scene* scene);
    };
}
