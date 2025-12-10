#pragma once
#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

VkDevice createLogicalDevice(VkInstance instance, VkPhysicalDevice physicalDevice);
void destroyLogicalDevice(VkDevice device);