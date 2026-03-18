#include "System.h"
#include "Scene.h"

namespace HBE
{
    System::System(Scene* scene) :
	context(*Application::instance->getContext()),
	input(*Application::instance->getInput()),
	window(*Application::instance->getWindow())
    {
        this->scene = scene;
    }
}
