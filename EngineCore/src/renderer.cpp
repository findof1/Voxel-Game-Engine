#include "renderer.hpp"

Renderer::Renderer(GLFWwindow *window) : window(window)
{
}

void Renderer::init()
{
  instance = createInstance(window);
}

Renderer::~Renderer()
{
  cleanup();
}

void Renderer::cleanup()
{
  destroyInstance(instance);
}