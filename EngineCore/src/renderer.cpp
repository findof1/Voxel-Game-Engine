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