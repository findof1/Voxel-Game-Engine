#pragma once
#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "vulkanSwapchain.hpp"

VkViewport makeViewport(VkExtent2D extent);
VkRect2D makeScissor(VkExtent2D extent);
void setViewport(VkCommandBuffer commandBuffer, VkViewport viewport);
void setScissor(VkCommandBuffer commandBuffer, VkRect2D scissor);
void draw(VkCommandBuffer commandBuffer, int vertexCount, int instanceCount = 1, int firstVertex = 0, int firstInstance = 1);