#pragma once
#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

VkSemaphore createSemaphore(VkDevice device);
void destroySemaphore(VkSemaphore semaphore, VkDevice device);

VkFence createFence(VkDevice device, VkFenceCreateFlags flags = 0);
void destroyFence(VkFence fence, VkDevice device);

void waitForFence(VkFence fence, VkDevice device);
void resetFence(VkFence fence, VkDevice device);