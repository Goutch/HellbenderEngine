#pragma once

#include "Core.h"
#include <cstdlib>

class HB_API Random {
public:
    static float range(float min,float max);
    static int range(int min,int max);
};

