#pragma once
#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "vulkanSwapchain.hpp"

void createSwapchainFramebuffers(VkRenderPass renderPass, SwapChainObjects &swapChainObjects, VkDevice device);
void destroySwapchainFramebuffers(SwapChainObjects &swapChainObjects, VkDevice device);

VkFramebuffer createFramebuffer(VkRenderPass renderPass, VkImageView attatchments, VkExtent2D extent, VkDevice device);
void destroyFramebuffer(VkFramebuffer framebuffer, VkDevice device);