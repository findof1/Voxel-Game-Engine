#include <chrono>
#include <cstring>
#include <cstring>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vulkanBufferObjects.hpp"
#include "vulkanBufferUtils.hpp"
#include "renderer.hpp"
#include "uniformData.hpp"

void createVertexBuffer(VkDeviceMemory &vertexBufferMemory, VkBuffer &vertexBuffer, VkDeviceSize bufferSize, const void *vertData, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice)
{

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, device, physicalDevice);

  void *data;
  vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, vertData, (size_t)bufferSize);
  vkUnmapMemory(device, stagingBufferMemory);

  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory, device, physicalDevice);

  copyBuffer(stagingBuffer, vertexBuffer, bufferSize, commandPool, graphicsQueue, device);

  destroyBuffer(stagingBufferMemory, stagingBuffer, device);
}

void bindVertexBuffer(VkBuffer vertexBuffer, VkCommandBuffer commandBuffer)
{
  VkBuffer vertexBuffers[] = {vertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
}

void createIndexBuffer(VkDeviceMemory &indexBufferMemory, VkBuffer &indexBuffer, const std::vector<uint32_t> &vertices, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice)
{
  VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, device, physicalDevice);

  void *data;
  vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, vertices.data(), (size_t)bufferSize);
  vkUnmapMemory(device, stagingBufferMemory);

  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory, device, physicalDevice);

  copyBuffer(stagingBuffer, indexBuffer, bufferSize, commandPool, graphicsQueue, device);

  destroyBuffer(stagingBufferMemory, stagingBuffer, device);
}

void bindIndexBuffer(VkBuffer indexBuffer, VkCommandBuffer commandBuffer)
{
  vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void createUniformBuffers(std::vector<VkBuffer> &uniformBuffers, std::vector<VkDeviceMemory> &uniformBuffersMemory, std::vector<void *> &uniformBuffersMapped, VkDevice device, VkPhysicalDevice physicalDevice)
{
  VkDeviceSize bufferSize = sizeof(UniformBufferObject);

  uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
  uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
  uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
  {
    createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i], device, physicalDevice);

    vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
  }
}

void destroyUniformBuffers(std::vector<VkBuffer> &uniformBuffers, std::vector<VkDeviceMemory> &uniformBuffersMemory, VkDevice device)
{
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
  {
    if (uniformBuffers[i] != VK_NULL_HANDLE)
    {
      vkDestroyBuffer(device, uniformBuffers[i], nullptr);
    }

    if (uniformBuffersMemory[i] != VK_NULL_HANDLE)
    {
      vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
    }
  }
}

void createStorageBuffer(VkDeviceSize bufferSize, VkBuffer &storageBuffer, VkDeviceMemory &storageBufferMemory, void *&storageBufferMapped, VkDevice device, VkPhysicalDevice physicalDevice)
{
  createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, storageBuffer, storageBufferMemory, device, physicalDevice);

  vkMapMemory(device, storageBufferMemory, 0, bufferSize, 0, &storageBufferMapped);
}

void destroyStorageBuffer(VkBuffer storageBuffer, VkDeviceMemory storageBufferMemory, VkDevice device)
{
  destroyBuffer(storageBufferMemory, storageBuffer, device);
}