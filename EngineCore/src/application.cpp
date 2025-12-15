#include "application.hpp"

Application::Application() : window(initWindow(windowWidth, windowHeight, "Voxel Game Engine")), renderer(window)
{
  if (!window)
  {
    std::cerr << "Failed to initialize window" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void Application::run()
{
  renderer.init();

  mainLoop();

  cleanup();
}

void Application::mainLoop()
{
  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    renderer.drawFrame();
  }
}

void Application::cleanup()
{
  closeWindow(window);
}