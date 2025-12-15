#include "renderer.hpp"

Renderer::Renderer(GLFWwindow *window) : window(window)
{
}

void Renderer::init()
{
  instance = createInstance(window);
  surface = createSurface(instance, window);
  physicalDevice = pickPhysicalDevice(surface, instance);
  device = createLogicalDevice(surface, physicalDevice, instance);
  graphicsQueue = createGraphicsQueue(surface, device, physicalDevice);
  presentQueue = createPresentQueue(surface, device, physicalDevice);
  swapChainObjects = createSwapChain(surface, device, physicalDevice, window);
  createImageViews(swapChainObjects, device);
  renderPass = createRenderPass(swapChainObjects, device);
  pipelineLayout = createPipelineLayout(device);
  pipeline = createGraphicsPipeline(pipelineLayout, renderPass, swapChainObjects, device, "shaders/vert.spv", "shaders/frag.spv");
  createSwapchainFramebuffers(renderPass, swapChainObjects, device);
  commandPool = createCommandPool(device, physicalDevice, surface, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  commandBuffer = createCommandBuffer(commandPool, device);

  imageAvailableSemaphore = createSemaphore(device);
  renderFinishedSemaphore = createSemaphore(device);
  inFlightFence = createFence(device, VK_FENCE_CREATE_SIGNALED_BIT);
}

void Renderer::drawFrame()
{
  waitForFence(inFlightFence, device);
  resetFence(inFlightFence, device);
  uint32_t imageIndex = acquireNextImageIndex(imageAvailableSemaphore, swapChainObjects.swapChain, device);
  resetCommandBuffer(commandBuffer);

  startRendering(imageIndex);
  drawObjects();
  endRendering();

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS)
  {
    std::cerr << "Failed to submit draw command buffer!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {swapChainObjects.swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr;
  vkQueuePresentKHR(presentQueue, &presentInfo);
}

void Renderer::startRendering(uint32_t imageIndex)
{
  beginCommandBuffer(commandBuffer);
  beginRenderPass(commandBuffer, swapChainObjects.swapChainFramebuffers[imageIndex], renderPass, swapChainObjects.swapChainExtent);
  bindGraphicsPipeline(commandBuffer, pipeline);

  VkViewport viewport = makeViewport(swapChainObjects.swapChainExtent);
  setViewport(commandBuffer, viewport);

  VkRect2D scissor = makeScissor(swapChainObjects.swapChainExtent);
  setScissor(commandBuffer, scissor);
}

void Renderer::drawObjects()
{
  vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

void Renderer::endRendering()
{
  endRenderPass(commandBuffer);
  endCommandBuffer(commandBuffer);
}

Renderer::~Renderer()
{
  cleanup();
}

void Renderer::cleanup()
{
  vkDeviceWaitIdle(device);

  destroyFence(inFlightFence, device);
  destroySemaphore(renderFinishedSemaphore, device);
  destroySemaphore(imageAvailableSemaphore, device);
  destroyCommandPool(commandPool, device);
  destroyPipeline(pipeline, device);
  destroyPipelineLayout(pipelineLayout, device);
  destroyRenderPass(renderPass, device);
  destroySwapchainFramebuffers(swapChainObjects, device);
  destroyImageViews(swapChainObjects.swapChainImageViews, device);
  destroySwapChain(swapChainObjects.swapChain, device);
  destroyLogicalDevice(device);
  destroySurface(surface, instance);
  destroyInstance(instance);

  if (window)
  {
    glfwDestroyWindow(window);
    glfwTerminate();
  }
}