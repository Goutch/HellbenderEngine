#pragma once


#include "Core.h"
#include "core/utility/Log.h"
#include "glm/glm.hpp"

namespace HBE{
    struct position:glm::vec3
    {

    };
    class HB_API Tester {
        position pos;
    public:
        position getPos();
        void setPos(float x,float y,float z);
        void setPos(position p);
        float getX();
        void printSomthing();
    };
}



