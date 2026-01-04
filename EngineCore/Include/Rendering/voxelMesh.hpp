#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>
#include "vertexData.hpp"
#include "texture.hpp"

struct VoxelDrawInfo
{
  int vertexOffset = UINT32_MAX;
  int vertexCount = 0;
  int indexOffset = UINT32_MAX;
  int indexCount = 0;
  int indirectIndex = UINT32_MAX;
};

struct UniformBufferObject;
class Renderer;
class VoxelMesh
{
public:
  Texture texture;
  VoxelDrawInfo drawInfo;

  VoxelMesh(Renderer &renderer);

  ~VoxelMesh()
  {
    Cleanup();
  };

  void Init(Texture texture, const std::vector<VoxelVertex> &verts, const std::vector<uint32_t> &inds, uint32_t gpuIndex);

  void Cleanup();

private:
  Renderer &renderer;
  std::vector<VoxelVertex> vertices;
  std::vector<uint32_t> indices;
};