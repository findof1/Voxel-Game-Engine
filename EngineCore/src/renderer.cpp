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
  swapChainObjects = createSwapChain(device, physicalDevice, surface, window);
  createImageViews(swapChainObjects, device);
  renderPass = createRenderPass(swapChainObjects, device);
  pipelineLayout = createPipelineLayout(device);
  pipeline = createGraphicsPipeline(pipelineLayout, renderPass, swapChainObjects, device, "shaders/vert.spv", "shaders/frag.spv");
  createSwapchainFramebuffers(renderPass, swapChainObjects, device);
  commandPool = createCommandPool(device, physicalDevice, surface, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  commandBuffers = createCommandBuffers(commandPool, device, MAX_FRAMES_IN_FLIGHT);

  imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
  {
    imageAvailableSemaphores[i] = createSemaphore(device);
    inFlightFences[i] = createFence(device, VK_FENCE_CREATE_SIGNALED_BIT);
  }

  renderFinishedSemaphores.resize(swapChainObjects.swapChainImages.size());
  for (size_t i = 0; i < renderFinishedSemaphores.size(); i++)
  {
    renderFinishedSemaphores[i] = createSemaphore(device);
  }
}

void Renderer::drawFrame()
{
  waitForFence(inFlightFences[currentFrame], device);

  uint32_t imageIndex;
  VkResult result = acquireNextImageIndex(imageIndex, imageAvailableSemaphores[currentFrame], swapChainObjects.swapChain, device);
  if (result == VK_ERROR_OUT_OF_DATE_KHR)
  {
    recreateSwapChain(renderPass, swapChainObjects, device, physicalDevice, surface, window);
    return; // stop drawing the frame if the swapchain is out of date
  }
  else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
  {
    std::cerr << "Failed to acquire swap chain image!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }

  resetFence(inFlightFences[currentFrame], device);
  resetCommandBuffer(commandBuffers[currentFrame]);

  startRendering(imageIndex);
  drawObjects();
  endRendering();

  submitFrame(imageAvailableSemaphores[currentFrame], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, renderFinishedSemaphores[imageIndex], inFlightFences[currentFrame], commandBuffers[currentFrame], graphicsQueue);

  result = presentFrame(imageIndex, renderFinishedSemaphores[imageIndex], swapChainObjects.swapChain, presentQueue);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
  {
    recreateSwapChain(renderPass, swapChainObjects, device, physicalDevice, surface, window);
  }
  else if (result != VK_SUCCESS)
  {
    std::cerr << "Failed to present swap chain image!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::startRendering(uint32_t imageIndex)
{
  beginCommandBuffer(commandBuffers[currentFrame]);
  beginRenderPass(commandBuffers[currentFrame], swapChainObjects.swapChainFramebuffers[imageIndex], renderPass, swapChainObjects.swapChainExtent);
  bindGraphicsPipeline(commandBuffers[currentFrame], pipeline);

  VkViewport viewport = makeViewport(swapChainObjects.swapChainExtent);
  setViewport(commandBuffers[currentFrame], viewport);

  VkRect2D scissor = makeScissor(swapChainObjects.swapChainExtent);
  setScissor(commandBuffers[currentFrame], scissor);
}

void Renderer::drawObjects()
{
  vkCmdDraw(commandBuffers[currentFrame], 3, 1, 0, 0);
}

void Renderer::endRendering()
{
  endRenderPass(commandBuffers[currentFrame]);
  endCommandBuffer(commandBuffers[currentFrame]);
}

Renderer::~Renderer()
{
  cleanup();
}

void Renderer::cleanup()
{
  vkDeviceWaitIdle(device);

  for (auto fence : inFlightFences)
  {
    destroyFence(fence, device);
  }

  for (auto semaphore : renderFinishedSemaphores)
  {
    destroySemaphore(semaphore, device);
  }

  for (auto semaphore : imageAvailableSemaphores)
  {
    destroySemaphore(semaphore, device);
  }

  destroyCommandPool(commandPool, device);
  destroyPipeline(pipeline, device);
  destroyPipelineLayout(pipelineLayout, device);
  destroyRenderPass(renderPass, device);
  cleanupSwapChain(swapChainObjects, device);
  destroyLogicalDevice(device);
  destroySurface(surface, instance);
  destroyInstance(instance);

  if (window)
  {
    glfwDestroyWindow(window);
    glfwTerminate();
  }
}