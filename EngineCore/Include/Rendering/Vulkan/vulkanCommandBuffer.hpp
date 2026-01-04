#pragma once
#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

// common flags: VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT for recording command buffers every frame. VK_COMMAND_POOL_CREATE_TRANSIENT_BIT for using command buffers that are recorded with new commands very often.
VkCommandPool createCommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPoolCreateFlags flags = 0);
void destroyCommandPool(VkCommandPool commandPool, VkDevice device);

std::vector<VkCommandBuffer> createCommandBuffers(VkCommandPool commandPool, VkDevice device, int count);
VkCommandBuffer createCommandBuffer(VkCommandPool commandPool, VkDevice device);
// note: no destroyer because the command pool automaticially destroys allocated command buffers

void beginCommandBuffer(VkCommandBuffer commandBuffer);
void endCommandBuffer(VkCommandBuffer commandBuffer);
void resetCommandBuffer(VkCommandBuffer commandBuffer);

void submitFrame(VkSemaphore waitSemaphore, VkPipelineStageFlags waitStage, VkSemaphore signalSemaphore, VkFence fence, VkCommandBuffer commandBuffer, VkQueue graphicsQueue);
VkResult presentFrame(uint32_t imageIndex, VkSemaphore waitSemaphore, VkSwapchainKHR swapchain, VkQueue presentQueue);