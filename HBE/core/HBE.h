#include <string>

class GLFWwindow;

class Scene;

class Clock;

class HBE {
    static GLFWwindow *window;
    static Clock *time;
    static int fps_counter;
    static float fps_timer;
public:
    static Scene *current_scene;

    static void init();

    static void run();

    static Scene *setScene(std::string path);

    static void quit();

    static void terminate();

    static float getTime();
private:
    static void printFPS(float);
};


