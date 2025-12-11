#pragma once
#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, VkInstance instance);
void destroyLogicalDevice(VkDevice device);