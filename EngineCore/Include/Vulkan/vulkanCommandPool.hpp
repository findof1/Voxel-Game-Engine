#pragma once
#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

// common flags: VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT for recording command buffers every frame. VK_COMMAND_POOL_CREATE_TRANSIENT_BIT for using command buffers that are recorded with new commands very often.
VkCommandPool createCommandPool(VkPhysicalDevice physicalDevice, VkPhysicalDevice device, VkCommandPoolCreateFlags flags = 0);
void destroyCommandPool(VkCommandPool commandPool, VkPhysicalDevice device);