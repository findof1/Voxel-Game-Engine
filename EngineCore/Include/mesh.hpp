#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>

struct Vertex;
struct UniformBufferObject;
class Renderer;
class Mesh
{
public:
  Mesh(Renderer &renderer);

  ~Mesh()
  {
    Cleanup();
  };

  void Init(const std::vector<Vertex> &verts, const std::vector<uint16_t> &inds);

  void Cleanup();

  void UpdateUBO(const UniformBufferObject &ubo);

  void Draw();

  VkBuffer GetVertexBuffer() const;
  VkBuffer GetIndexBuffer() const;
  uint32_t GetIndexCount() const;
  VkDescriptorSet GetDescriptorSet(uint32_t frameIndex) const;

private:
  Renderer &renderer;

  VkBuffer vertexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;

  VkBuffer indexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void *> uniformBuffersMapped;

  std::vector<VkDescriptorSet> descriptorSets;

  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;
};