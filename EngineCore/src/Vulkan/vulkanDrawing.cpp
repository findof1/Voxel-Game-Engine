#include "vulkanDrawing.hpp"

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
