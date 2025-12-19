#include "vulkanDrawing.hpp"

VkViewport makeViewport(VkExtent2D extent)
{
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(extent.width);
  viewport.height = static_cast<float>(extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  return viewport;
}

VkRect2D makeScissor(VkExtent2D extent)
{
  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = extent;

  return scissor;
}

void setViewport(VkCommandBuffer commandBuffer, VkViewport viewport)
{
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
}

void setScissor(VkCommandBuffer commandBuffer, VkRect2D scissor)
{
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void draw(VkCommandBuffer commandBuffer, int vertexCount, int instanceCount, int firstVertex, int firstInstance)
{
  vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void drawIndexed(VkCommandBuffer commandBuffer, int indicesCount, int instanceCount, int firstVertex, int firstInstance)
{
  vkCmdDrawIndexed(commandBuffer, indicesCount, 1, 0, 0, 0);
}