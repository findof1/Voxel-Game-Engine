#pragma once
#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "vertexData.hpp"

// note: use destroyBuffer to destroy all buffers

void createEmptyVertexBuffer(VkDeviceMemory &vertexBufferMemory, VkBuffer &vertexBuffer, VkDeviceSize bufferSize, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice);
void uploadToVertexBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, const void *srcData, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice);
void createVertexBuffer(VkDeviceMemory &vertexBufferMemory, VkBuffer &vertexBuffer, VkDeviceSize bufferSize, const void *vertData, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice);

void bindVertexBuffer(VkBuffer vertexBuffer, VkCommandBuffer commandBuffer);

void createEmptyIndexBuffer(VkDeviceMemory &indexBufferMemory, VkBuffer &indexBuffer, VkDeviceSize bufferSize, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice);
void uploadToIndexBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, const void *srcData, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice);
void createIndexBuffer(VkDeviceMemory &indexBufferMemory, VkBuffer &indexBuffer, const std::vector<uint32_t> &vertices, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice);

void bindIndexBuffer(VkBuffer indexBuffer, VkCommandBuffer commandBuffer);

void createUniformBuffers(std::vector<VkBuffer> &uniformBuffers, std::vector<VkDeviceMemory> &uniformBuffersMemory, std::vector<void *> &uniformBuffersMapped, VkDevice device, VkPhysicalDevice physicalDevice);
void destroyUniformBuffers(std::vector<VkBuffer> &uniformBuffers, std::vector<VkDeviceMemory> &uniformBuffersMemory, VkDevice device);

void createStorageBuffer(VkDeviceSize bufferSize, VkBuffer &storageBuffer, VkDeviceMemory &storageBufferMemory, void *&storageBufferMapped, VkDevice device, VkPhysicalDevice physicalDevice);
void destroyStorageBuffer(VkBuffer storageBuffer, VkDeviceMemory storageBufferMemory, VkDevice device);

void createEmptyIndirectBuffer(VkDeviceMemory &indirectBufferMemory, VkBuffer &indirectBuffer, VkDeviceSize bufferSize, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice);
void uploadToIndirectBuffer(VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, const void *srcData, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice);