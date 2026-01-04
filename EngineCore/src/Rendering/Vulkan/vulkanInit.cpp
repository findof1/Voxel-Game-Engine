#include <vector>
#include <cstring>

#include "vulkanInit.hpp"
#include "renderer.hpp"
#include "application.hpp"

void framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
  auto app = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
  app->renderer.framebufferResized = true;
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
  auto app = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
  if (app->firstMouse)
  {
    app->lastX = (float)xpos;
    app->lastY = (float)ypos;
    app->firstMouse = false;
  }

  float xoffset = (float)xpos - app->lastX;
  float yoffset = app->lastY - (float)ypos;

  app->lastX = (float)xpos;
  app->lastY = (float)ypos;

  app->camera.processMouseMovement(xoffset, -yoffset);
}

GLFWwindow *initWindow(const int w, const int h, std::string windowName, void *userPointer)
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

  if (userPointer)
  {
    glfwSetWindowUserPointer(window, userPointer);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
  if (enableValidationLayers && !checkValidationLayerSupport())
  {
    std::cerr << "Validation layers requested, but not available!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }

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

  if (enableValidationLayers)
  {
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  }
  else
  {
    createInfo.enabledLayerCount = 0;
  }

  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;

  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> requiredExtensions;

  for (uint32_t i = 0; i < glfwExtensionCount; i++)
  {
    requiredExtensions.emplace_back(glfwExtensions[i]);
  }

#ifdef __APPLE__
  requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

  createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();

  auto res = vkCreateInstance(&createInfo, nullptr, &instance);
  if (res != VK_SUCCESS)
  {
    std::cerr << "Failed to create Vulkan instance. Error: " << res << std::endl;
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
  if (instance != VK_NULL_HANDLE)
  {
    vkDestroyInstance(instance, nullptr);
  }
}

bool checkValidationLayerSupport()
{
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : validationLayers)
  {
    bool layerFound = false;

    for (const auto &layerProperties : availableLayers)
    {
      if (strcmp(layerName, layerProperties.layerName) == 0)
      {
        layerFound = true;
        break;
      }
    }

    if (!layerFound)
    {
      return false;
    }
  }

  return true;
}