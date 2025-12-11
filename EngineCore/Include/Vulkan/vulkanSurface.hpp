#pragma once
#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

VkSurfaceKHR createSurface(VkDevice device, VkInstance instance); // note: steps are different for windows and linux, so use #ifdef to add both
void destroySurface(VkSurfaceKHR surface, VkInstance instance);

VkQueue createPresentQueue(VkSurfaceKHR surface, VkDevice device, VkPhysicalDevice physicalDevice);
// note: no destroyer for the present queue because they are automaticially destroyed with the VkDevice