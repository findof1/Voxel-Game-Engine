#pragma once
#include <iostream>
#include <optional>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

struct QueueFamilyIndices
{
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete()
  {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

VkPhysicalDevice pickPhysicalDevice(VkSurfaceKHR surface, VkInstance instance);
// note: no destroyer for physical devices because they get automaticially destroyed when the instance is destroyed

VkDevice createLogicalDevice(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkInstance instance);
void destroyLogicalDevice(VkDevice device);

int rateDeviceSuitability(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice);
bool checkDeviceExtensionSupport(VkPhysicalDevice device);
QueueFamilyIndices findQueueFamilies(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice);
