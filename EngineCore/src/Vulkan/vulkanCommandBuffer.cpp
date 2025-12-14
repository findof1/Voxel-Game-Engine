#include "vulkanCommandBuffer.hpp"
#include "vulkanDevice.hpp"

VkCommandPool createCommandPool(VkPhysicalDevice physicalDevice, VkDevice device, VkCommandPoolCreateFlags flags = 0)
{
  QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

  VkCommandPool commandPool;
  if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
  {
    std::cerr << "Failed to create command pool!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }
  return commandPool;
}

void destroyCommandPool(VkCommandPool commandPool, VkDevice device)
{
  if (commandPool != VK_NULL_HANDLE)
  {
    vkDestroyCommandPool(device, commandPool, nullptr);
  }
}

VkCommandBuffer createCommandBuffer(VkCommandPool commandPool, VkDevice device)
{
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS)
  {
    std::cerr << "Failed to create command buffers!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }
}

void beginCommandBuffer(VkCommandBuffer commandBuffer)
{
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
  {
    std::cerr << "Failed to begin recording command buffer!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }
}

void endCommandBuffer(VkCommandBuffer commandBuffer)
{
  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
  {
    std::cerr << "Failed to record command buffer!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }
}

void resetCommandBuffer(VkCommandBuffer commandBuffer)
{
  vkResetCommandBuffer(commandBuffer, 0);
}
