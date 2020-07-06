#include <string>

class GLFWwindow;

class Scene;

class Timer;

class HBE {
    static GLFWwindow *window;
    static Timer *time;
    static int fps_counter;
    static float fps_timer;
public:
    static void init();

    static void run();

    static Scene *setScene(std::string path);

    static void quit();

    static void terminate();

    static float getTime();

    static Scene *current_scene;

    static void printFPS(float);
};


