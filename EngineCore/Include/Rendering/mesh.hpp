#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>
#include "vertexData.hpp"
#include "texture.hpp"

struct UniformBufferObject;
class Renderer;
class Mesh
{
public:
  Texture texture;

  Mesh(Renderer &renderer);

  ~Mesh()
  {
    Cleanup();
  };

  void Init(Texture texture, const std::vector<Vertex> &verts, const std::vector<uint32_t> &inds);

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

  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
};