#pragma once
#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "vulkanSwapchain.hpp"

VkRenderPass createRenderPass(const SwapChainObjects &swapChainObjects, VkDevice device);
void destroyRenderPass(VkRenderPass renderPass, VkDevice device);

void beginRenderPass(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkRenderPass renderPass, VkExtent2D extent, VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}}, VkOffset2D offset = {0, 0});
void endRenderPass(VkCommandBuffer commandBuffer);