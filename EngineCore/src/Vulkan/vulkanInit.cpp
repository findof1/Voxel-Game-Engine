#include "vulkanInit.hpp"
#include <vector>

GLFWwindow *initWindow(const int w, const int h, std::string windowName)
{
  if (!glfwInit())
  {
    std::cerr << "Failed to initialize GLFW\n";
    return nullptr;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  GLFWwindow *window = glfwCreateWindow(w, h, windowName.c_str(), nullptr, nullptr);
  if (!window)
  {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return nullptr;
  }

  return window;
}

void closeWindow(GLFWwindow *window)
{
  glfwDestroyWindow(window);
  glfwTerminate();
}

VkInstance createInstance(GLFWwindow *window)
{
  VkInstance instance;

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "VoxelGameEngine";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "NoEngine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;

  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> requiredExtensions;

  for (uint32_t i = 0; i < glfwExtensionCount; i++)
  {
    requiredExtensions.emplace_back(glfwExtensions[i]);
  }

  requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

  createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

  createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
  {
    std::cerr << "Failed to create Vulkan instance" << std::endl;
    glfwDestroyWindow(window);
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }

  return instance;
}

void destroyInstance(VkInstance instance)
{
  vkDestroyInstance(instance, nullptr);
}

bool checkValidationLayerSupport()
{
}