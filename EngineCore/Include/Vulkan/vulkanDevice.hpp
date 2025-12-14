#pragma once
#include <iostream>
#include <optional>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

struct QueueFamilyIndices
{
  std::optional<uint32_t> graphicsFamily;

  bool isComplete()
  {
    return graphicsFamily.has_value();
  }
};

VkPhysicalDevice pickPhysicalDevice(VkInstance instance);
// note: no destroyer for physical devices because they get automaticially destroyed when the instance is destroyed

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, VkInstance instance);
void destroyLogicalDevice(VkDevice device);

bool isDeviceSuitable(VkPhysicalDevice physicalDevice);
bool checkDeviceExtensionSupport(VkPhysicalDevice device);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);
