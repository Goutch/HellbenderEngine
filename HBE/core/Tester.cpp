//
// Created by User on 2021-10-11.
//

#include "Tester.h"
namespace HBE{
    position Tester::getPos()
    {
        return pos;
    }
    float Tester::getX()
    {
        return pos.x;
    }
    void Tester::printSomthing()
    {
        Log::debug("something");
    }

    void Tester::setPos(float x, float y, float z) {
        pos.x=x;
        pos.y=y;
        pos.z=z;
    }

    void Tester::setPos(position p) {
        pos=p;
    }
}
