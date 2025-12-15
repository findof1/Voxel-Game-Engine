#pragma once
#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

VkSurfaceKHR createSurface(VkInstance instance, GLFWwindow *window);
void destroySurface(VkSurfaceKHR surface, VkInstance instance);

VkQueue createGraphicsQueue(VkSurfaceKHR surface, VkDevice device, VkPhysicalDevice physicalDevice);
VkQueue createPresentQueue(VkSurfaceKHR surface, VkDevice device, VkPhysicalDevice physicalDevice);
// note: no destroyer for the queues because they are automaticially destroyed with the VkDevice