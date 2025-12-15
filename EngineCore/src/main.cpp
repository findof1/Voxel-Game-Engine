#include <iostream>

#include <vulkan/vulkan.h>
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#else
#include <GLFW/glfw3.h>
#endif

#include "application.hpp"

int main()
{
  {
    Application engine;
    engine.run();
  }

  std::cout << "Press Enter to exit" << std::endl;
  std::cin.get();
  return 0;
}
