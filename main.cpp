#include "HBE.h"
#include "editor/Editor.h"
int main() {
    Configs::setWindowTitle("3D view");

    HBE::init();
    Editor e;
    e.start();
    HBE::run();
    e.terminate();
    HBE::terminate();
}