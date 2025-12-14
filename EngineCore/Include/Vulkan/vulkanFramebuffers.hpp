#pragma once
#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "vulkanSwapchain.hpp"

void createSwapchainFramebuffers(VkRenderPass renderPass, SwapChainObjects &swapChainObjects, VkDevice device);
void destroySwapchainFramebuffers(SwapChainObjects &swapChainObjects, VkDevice device);

VkFramebuffer createFramebuffer(VkRenderPass renderPass, std::vector<VkImageView> &attachments, VkExtent2D extent, VkDevice device);
void destroyFramebuffer(VkFramebuffer framebuffer, VkDevice device);