#include "System.h"
#include "Scene.h"

namespace HBE
{
    System::System(Scene* scene)
    {
        context = Application::instance->getContext();
        this->scene = scene;
    }
}
