#include "application.hpp"
#include <chrono>
#include <iostream>

Application::Application() : window(initWindow(windowWidth, windowHeight, "Voxel Game Engine", this)), renderer(window)
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
  uint32_t frameCount = 0;
  double fps = 0.0;
  auto lastTime = std::chrono::high_resolution_clock::now();
  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    renderer.drawFrame();

    frameCount++;

    auto currentTime = std::chrono::high_resolution_clock::now();
    double deltaTime = std::chrono::duration<double, std::chrono::seconds::period>(currentTime - lastTime).count();

    if (deltaTime >= 1.0)
    {
      fps = frameCount / deltaTime;
      std::cout << "FPS: " << fps << std::endl;

      frameCount = 0;
      lastTime = currentTime;
    }

    std::string title = "Voxel Game Engine - FPS: " + std::to_string(fps);
    glfwSetWindowTitle(window, title.c_str());
  }
}

void Application::cleanup()
{
  closeWindow(window);
}