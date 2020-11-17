#include "Application.h"

int main()
{
    HBE::init();
    HBE::scene->instantiate();
    HBE::run();
    HBE::terminate();
}