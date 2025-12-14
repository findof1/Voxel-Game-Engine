#include "vulkanFramebuffers.hpp"

void createSwapchainFramebuffers(VkRenderPass renderPass, SwapChainObjects &swapChainObjects, VkDevice device)
{
  swapChainObjects.swapChainFramebuffers.resize(swapChainObjects.swapChainImageViews.size());

  for (size_t i = 0; i < swapChainObjects.swapChainImageViews.size(); i++)
  {
    std::vector<VkImageView> attachments = {swapChainObjects.swapChainImageViews[i]};

    swapChainObjects.swapChainFramebuffers[i] = createFramebuffer(renderPass, attachments, swapChainObjects.swapChainExtent, device);
  }
}

void destroySwapchainFramebuffers(SwapChainObjects &swapChainObjects, VkDevice device)
{
  for (auto framebuffer : swapChainObjects.swapChainFramebuffers)
  {
    destroyFramebuffer(framebuffer, device);
  }
  swapChainObjects.swapChainFramebuffers.clear();
}

VkFramebuffer createFramebuffer(VkRenderPass renderPass, const std::vector<VkImageView> &attachments, VkExtent2D extent, VkDevice device)
{
  VkFramebufferCreateInfo framebufferInfo{};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass = renderPass;
  framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  ;
  framebufferInfo.pAttachments = attachments.data();
  framebufferInfo.width = extent.width;
  framebufferInfo.height = extent.height;
  framebufferInfo.layers = 1;

  VkFramebuffer framebuffer;
  if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
  {
    std::cerr << "Failed to create framebuffer!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }
  return framebuffer;
}

void destroyFramebuffer(VkFramebuffer framebuffer, VkDevice device)
{
  if (framebuffer != VK_NULL_HANDLE)
  {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  }
}
