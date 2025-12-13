#pragma once
#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

VkCommandBuffer createCommandBuffer(VkCommandPool commandPool, VkPhysicalDevice device);
// note: no destroyer because the command pool automaticially destroys allocated command buffers

void beginCommandBuffer(VkCommandBuffer commandBuffer);
void endCommandBuffer(VkCommandBuffer commandBuffer);