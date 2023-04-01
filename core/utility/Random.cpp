//
// Created by User on 04-Nov.-2020.
//

#include "Random.h"
namespace HBE {
    float Random::floatRange(float min, float max) {
        return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
    }

    int Random::intRange(int min, int max) {
        return min + (rand() % (max - min));
    }
}
