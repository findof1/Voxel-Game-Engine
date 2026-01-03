#include "voxelMesh.hpp"
#include "renderer.hpp"
#include "uniformData.hpp"
#include <cstring>
#include "vulkanBufferUtils.hpp"
#include "camera.hpp"

VoxelMesh::VoxelMesh(Renderer &renderer) : renderer(renderer)
{
}

void VoxelMesh::Init(Texture texture, const std::vector<VoxelVertex> &verts, const std::vector<uint32_t> &inds, uint32_t gpuIndex)
{
  this->texture = texture;
  vertices = verts;
  indices = inds;

  drawInfo.vertexCount = static_cast<uint32_t>(vertices.size());
  drawInfo.indexCount = static_cast<uint32_t>(indices.size());

  drawInfo.vertexOffset = renderer.voxelBuffers.vertexAlloc.allocate(drawInfo.vertexCount);
  drawInfo.indexOffset = renderer.voxelBuffers.indexAlloc.allocate(drawInfo.indexCount);

  assert(drawInfo.vertexOffset != UINT32_MAX);
  assert(drawInfo.indexOffset != UINT32_MAX);

  uploadToVertexBuffer(renderer.voxelBuffers.vertexBuffer, drawInfo.vertexOffset * sizeof(VoxelVertex), drawInfo.vertexCount * sizeof(VoxelVertex), vertices.data(), renderer.commandPool, renderer.graphicsQueue, renderer.device, renderer.physicalDevice);

  uploadToIndexBuffer(renderer.voxelBuffers.indexBuffer, drawInfo.indexOffset * sizeof(uint32_t), drawInfo.indexCount * sizeof(uint32_t), indices.data(), renderer.commandPool, renderer.graphicsQueue, renderer.device, renderer.physicalDevice);

  drawInfo.indirectIndex = renderer.voxelBuffers.indirectAlloc.allocate(1);
  assert(drawInfo.indirectIndex != UINT32_MAX);

  VkDrawIndexedIndirectCommand cmd{};
  cmd.indexCount = drawInfo.indexCount;
  cmd.instanceCount = 1;
  cmd.firstIndex = drawInfo.indexOffset;
  cmd.vertexOffset = drawInfo.vertexOffset;
  cmd.firstInstance = gpuIndex;

  uploadToIndirectBuffer(renderer.voxelBuffers.indirectBuffer, drawInfo.indirectIndex * sizeof(VkDrawIndexedIndirectCommand), sizeof(VkDrawIndexedIndirectCommand), &cmd, renderer.commandPool, renderer.graphicsQueue, renderer.device, renderer.physicalDevice);

  renderer.voxelBuffers.drawCount++;
  // renderer.voxelBuffers.indirectCommands[drawInfo.indirectIndex] = cmd;
}

void VoxelMesh::Cleanup()
{
  if (drawInfo.indirectIndex != UINT32_MAX)
  {
    VkDrawIndexedIndirectCommand cmd{};
    cmd.indexCount = 0;
    uploadToIndirectBuffer(
        renderer.voxelBuffers.indirectBuffer,
        drawInfo.indirectIndex * sizeof(VkDrawIndexedIndirectCommand),
        sizeof(VkDrawIndexedIndirectCommand),
        &cmd,
        renderer.commandPool,
        renderer.graphicsQueue,
        renderer.device,
        renderer.physicalDevice);

    renderer.voxelBuffers.indirectAlloc.free(drawInfo.indirectIndex, 1);
    drawInfo.indirectIndex = UINT32_MAX;
    renderer.voxelBuffers.drawCount--;
  }

  if (drawInfo.vertexOffset != UINT32_MAX)
  {
    renderer.voxelBuffers.vertexAlloc.free(drawInfo.vertexOffset, drawInfo.vertexCount);
    drawInfo.vertexOffset = UINT32_MAX;
    drawInfo.vertexCount = 0;
    vertices.clear();
  }

  if (drawInfo.indexOffset != UINT32_MAX)
  {
    renderer.voxelBuffers.indexAlloc.free(drawInfo.indexOffset, drawInfo.indexCount);
    drawInfo.indexOffset = UINT32_MAX;
    drawInfo.indexCount = 0;
    indices.clear();
  }
}