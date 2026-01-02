#include "mesh.hpp"
#include "renderer.hpp"
#include "uniformData.hpp"
#include <cstring>
#include "vulkanBufferUtils.hpp"

Mesh::Mesh(Renderer &renderer) : renderer(renderer)
{
}

void Mesh::Init(Texture texture, const std::vector<Vertex> &verts, const std::vector<uint32_t> &inds)
{
  this->texture = texture;
  vertices = verts;
  indices = inds;

  createVertexBuffer(vertexBufferMemory, vertexBuffer, sizeof(vertices[0]) * vertices.size(), vertices.data(), renderer.commandPool, renderer.graphicsQueue, renderer.device, renderer.physicalDevice);

  createIndexBuffer(indexBufferMemory, indexBuffer, indices, renderer.commandPool, renderer.graphicsQueue, renderer.device, renderer.physicalDevice);
}

void Mesh::Cleanup()
{
  VkDevice device = renderer.device;

  vkDeviceWaitIdle(device);

  destroyBuffer(indexBufferMemory, indexBuffer, device);
  indexBuffer = VK_NULL_HANDLE;
  indexBufferMemory = VK_NULL_HANDLE;
  destroyBuffer(vertexBufferMemory, vertexBuffer, device);
  vertexBuffer = VK_NULL_HANDLE;
  vertexBufferMemory = VK_NULL_HANDLE;
}

void Mesh::Draw()
{
  uint32_t currentFrame = renderer.currentFrame;
  VkCommandBuffer commandBuffer = renderer.commandBuffers[currentFrame];
  bindVertexBuffer(vertexBuffer, commandBuffer);
  bindIndexBuffer(indexBuffer, commandBuffer);
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