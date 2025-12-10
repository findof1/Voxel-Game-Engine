#include "renderer.hpp"

class Application
{
public:
  int windowWidth = 800;
  int windowHeight = 600;
  GLFWwindow *window;
  Renderer renderer;

  Application();
  void run();

private:
  void mainLoop();
  void cleanup();
};
