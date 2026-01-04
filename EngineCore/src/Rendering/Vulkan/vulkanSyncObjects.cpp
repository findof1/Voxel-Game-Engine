#include "vulkanSyncObjects.hpp"

VkSemaphore createSemaphore(VkDevice device)
{
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkSemaphore semaphore;
  if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS)
  {
    std::cerr << "Failed to create semaphore!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }

  return semaphore;
}

void destroySemaphore(VkSemaphore semaphore, VkDevice device)
{
  if (semaphore != VK_NULL_HANDLE)
  {
    vkDestroySemaphore(device, semaphore, nullptr);
  }
}

VkFence createFence(VkDevice device, VkFenceCreateFlags flags)
{
  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = flags;

  VkFence fence;
  if (vkCreateFence(device, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
  {
    std::cerr << "Failed to create semaphore!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }

  return fence;
}

void destroyFence(VkFence fence, VkDevice device)
{
  if (fence != VK_NULL_HANDLE)
  {
    vkDestroyFence(device, fence, nullptr);
  }
}

void waitForFence(VkFence fence, VkDevice device)
{
  vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
}

void resetFence(VkFence fence, VkDevice device)
{
  vkResetFences(device, 1, &fence);
}