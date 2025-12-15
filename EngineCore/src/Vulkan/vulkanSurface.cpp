#include <vector>

#include "vulkanSurface.hpp"
#include "vulkanDevice.hpp"

VkSurfaceKHR createSurface(VkInstance instance, GLFWwindow *window)
{
  VkSurfaceKHR surface;
  if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
  {
    std::cerr << "Failed to create window surface!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }
  return surface;
}

void destroySurface(VkSurfaceKHR surface, VkInstance instance)
{
  if (surface != VK_NULL_HANDLE)
  {
    vkDestroySurfaceKHR(instance, surface, nullptr);
  }
}

VkQueue createGraphicsQueue(VkSurfaceKHR surface, VkDevice device, VkPhysicalDevice physicalDevice)
{
  QueueFamilyIndices indices = findQueueFamilies(surface, physicalDevice);
  VkQueue graphicsQueue;
  vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
  return graphicsQueue;
}

VkQueue createPresentQueue(VkSurfaceKHR surface, VkDevice device, VkPhysicalDevice physicalDevice)
{
  QueueFamilyIndices indices = findQueueFamilies(surface, physicalDevice);
  VkQueue presentQueue;
  vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
  return presentQueue;
}
