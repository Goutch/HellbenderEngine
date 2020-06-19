#include <string>

class GLFWwindow;
class Scene;
class Timer;

class PrototypeEngine {
    static GLFWwindow* window;
    static Timer* time;
public:
    static void init();
    static void run();
    static Scene* setScene(std::string path);
    static void quit();
    static void terminate();
    static float getTime();
    static Scene* current_scene;
};


