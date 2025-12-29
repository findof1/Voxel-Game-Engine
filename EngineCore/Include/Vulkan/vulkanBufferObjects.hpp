#pragma once
#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "vertexData.hpp"

// note: use destroyBuffer to destroy all buffers

void createVertexBuffer(VkDeviceMemory &vertexBufferMemory, VkBuffer &vertexBuffer, VkDeviceSize bufferSize, const void *vertData, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice);

void bindVertexBuffer(VkBuffer vertexBuffer, VkCommandBuffer commandBuffer);

void createIndexBuffer(VkDeviceMemory &indexBufferMemory, VkBuffer &indexBuffer, const std::vector<uint16_t> &vertices, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice);

void bindIndexBuffer(VkBuffer indexBuffer, VkCommandBuffer commandBuffer);

void createUniformBuffers(std::vector<VkBuffer> &uniformBuffers, std::vector<VkDeviceMemory> &uniformBuffersMemory, std::vector<void *> &uniformBuffersMapped, VkDevice device, VkPhysicalDevice physicalDevice);
void destroyUniformBuffers(std::vector<VkBuffer> &uniformBuffers, std::vector<VkDeviceMemory> &uniformBuffersMemory, VkDevice device);