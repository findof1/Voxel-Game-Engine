#include "mesh.hpp"
#include "renderer.hpp"
#include "uniformData.hpp"
#include <cstring>
#include "vulkanBufferUtils.hpp"

Mesh::Mesh(Renderer &renderer) : renderer(renderer)
{
}

void Mesh::Init(Texture texture, const std::vector<Vertex> &verts, const std::vector<uint16_t> &inds)
{
  vertices = verts;
  indices = inds;

  createVertexBuffer(vertexBufferMemory, vertexBuffer, sizeof(vertices[0]) * vertices.size(), vertices.data(), renderer.commandPool, renderer.graphicsQueue, renderer.device, renderer.physicalDevice);

  createIndexBuffer(indexBufferMemory, indexBuffer, indices, renderer.commandPool, renderer.graphicsQueue, renderer.device, renderer.physicalDevice);

  createUniformBuffers(uniformBuffers, uniformBuffersMemory, uniformBuffersMapped, renderer.device, renderer.physicalDevice);

  createDescriptorSets(descriptorSets, uniformBuffers, texture.view, texture.sampler, renderer.descriptorPool, renderer.descriptorSetLayout, renderer.device);
}

void Mesh::Cleanup()
{
  VkDevice device = renderer.device;

  vkDeviceWaitIdle(device);

  if (!descriptorSets.empty())
  {
    vkFreeDescriptorSets(device, renderer.descriptorPool, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data());
    descriptorSets.clear();
  }

  for (size_t i = 0; i < uniformBuffers.size(); ++i)
  {
    destroyBuffer(uniformBuffersMemory[i], uniformBuffers[i], device);
  }

  uniformBuffers.clear();
  uniformBuffersMemory.clear();
  uniformBuffersMapped.clear();

  destroyBuffer(indexBufferMemory, indexBuffer, device);
  indexBuffer = VK_NULL_HANDLE;
  indexBufferMemory = VK_NULL_HANDLE;
  destroyBuffer(vertexBufferMemory, vertexBuffer, device);
  vertexBuffer = VK_NULL_HANDLE;
  vertexBufferMemory = VK_NULL_HANDLE;
}

void Mesh::UpdateUBO(const UniformBufferObject &ubo)
{
  std::memcpy(uniformBuffersMapped[renderer.currentFrame], &ubo, sizeof(UniformBufferObject));
}

void Mesh::Draw()
{
  uint32_t currentFrame = renderer.currentFrame;
  VkCommandBuffer commandBuffer = renderer.commandBuffers[currentFrame];
  bindVertexBuffer(vertexBuffer, commandBuffer);
  bindIndexBuffer(indexBuffer, commandBuffer);
  bindDescriptorSets(descriptorSets[currentFrame], commandBuffer, renderer.pipelineLayout);
  drawIndexed(commandBuffer, indices.size());
}

VkBuffer Mesh::GetVertexBuffer() const
{
  return vertexBuffer;
}

VkBuffer Mesh::GetIndexBuffer() const
{
  return indexBuffer;
}

uint32_t Mesh::GetIndexCount() const
{
  return static_cast<uint32_t>(indices.size());
}

VkDescriptorSet Mesh::GetDescriptorSet(uint32_t frameIndex) const
{
  return descriptorSets[frameIndex];
}