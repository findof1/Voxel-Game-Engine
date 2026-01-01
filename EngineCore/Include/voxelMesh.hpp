#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>
#include "vertexData.hpp"
#include "texture.hpp"

struct UniformBufferObject;
class Renderer;
class VoxelMesh
{
public:
  Texture texture;

  VoxelMesh(Renderer &renderer);

  ~VoxelMesh()
  {
    Cleanup();
  };

  void Init(Texture texture, const std::vector<VoxelVertex> &verts, const std::vector<uint16_t> &inds);

  void Cleanup();

  void Draw();

  VkBuffer GetVertexBuffer() const;
  VkBuffer GetIndexBuffer() const;
  uint32_t GetIndexCount() const;

private:
  Renderer &renderer;

  VkBuffer vertexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;

  VkBuffer indexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

  std::vector<VoxelVertex> vertices;
  std::vector<uint16_t> indices;
};