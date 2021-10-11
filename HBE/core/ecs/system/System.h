#pragma once
#include "Core.h"
#include "vector"
namespace HBE{
    class System {
    public:
        virtual void update(float delta)=0;
        virtual void draw()=0;
        virtual void init()=0;
    };
}


