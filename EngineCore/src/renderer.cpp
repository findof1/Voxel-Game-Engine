#include "renderer.hpp"

const std::vector<Vertex> vertices = {
    // Front (+Z)
    {{-0.5f, -0.5f, 0.5f}, {1, 0, 0}, {0, 0}},
    {{0.5f, -0.5f, 0.5f}, {1, 0, 0}, {1, 0}},
    {{0.5f, 0.5f, 0.5f}, {1, 0, 0}, {1, 1}},
    {{-0.5f, 0.5f, 0.5f}, {1, 0, 0}, {0, 1}},

    // Back (-Z)
    {{0.5f, -0.5f, -0.5f}, {0, 1, 0}, {0, 0}},
    {{-0.5f, -0.5f, -0.5f}, {0, 1, 0}, {1, 0}},
    {{-0.5f, 0.5f, -0.5f}, {0, 1, 0}, {1, 1}},
    {{0.5f, 0.5f, -0.5f}, {0, 1, 0}, {0, 1}},

    // Left (-X)
    {{-0.5f, -0.5f, -0.5f}, {0, 0, 1}, {0, 0}},
    {{-0.5f, -0.5f, 0.5f}, {0, 0, 1}, {1, 0}},
    {{-0.5f, 0.5f, 0.5f}, {0, 0, 1}, {1, 1}},
    {{-0.5f, 0.5f, -0.5f}, {0, 0, 1}, {0, 1}},

    // Right (+X)
    {{0.5f, -0.5f, 0.5f}, {1, 1, 0}, {0, 0}},
    {{0.5f, -0.5f, -0.5f}, {1, 1, 0}, {1, 0}},
    {{0.5f, 0.5f, -0.5f}, {1, 1, 0}, {1, 1}},
    {{0.5f, 0.5f, 0.5f}, {1, 1, 0}, {0, 1}},

    // Top (+Y)
    {{-0.5f, 0.5f, 0.5f}, {0, 1, 1}, {0, 0}},
    {{0.5f, 0.5f, 0.5f}, {0, 1, 1}, {1, 0}},
    {{0.5f, 0.5f, -0.5f}, {0, 1, 1}, {1, 1}},
    {{-0.5f, 0.5f, -0.5f}, {0, 1, 1}, {0, 1}},

    // Bottom (-Y)
    {{-0.5f, -0.5f, -0.5f}, {1, 0, 1}, {0, 0}},
    {{0.5f, -0.5f, -0.5f}, {1, 0, 1}, {1, 0}},
    {{0.5f, -0.5f, 0.5f}, {1, 0, 1}, {1, 1}},
    {{-0.5f, -0.5f, 0.5f}, {1, 0, 1}, {0, 1}}};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20};

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
  renderPass = createRenderPass(swapChainObjects, device, physicalDevice);
  descriptorSetLayout = createDescriptorSetLayout(device);
  pipelineLayout = createPipelineLayout(descriptorSetLayout, device);
  pipeline = createGraphicsPipeline(pipelineLayout, renderPass, swapChainObjects, device, "shaders/vert.spv", "shaders/frag.spv");
  commandPool = createCommandPool(device, physicalDevice, surface, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  commandBuffers = createCommandBuffers(commandPool, device, MAX_FRAMES_IN_FLIGHT);
  createDepthResources(swapChainObjects, commandPool, graphicsQueue, device, physicalDevice);
  createSwapchainFramebuffers(renderPass, swapChainObjects, device);
  descriptorPool = createDescriptorPool(device);

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

  createTextureImage(textureImage, textureImageMemory, "Assets/textures/wood.png", commandPool, graphicsQueue, device, physicalDevice);
  textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, device, VK_IMAGE_ASPECT_COLOR_BIT);
  textureSampler = createTextureSampler(device, physicalDevice);
  createVertexBuffer(vertexBufferMemory, vertexBuffer, vertices, commandPool, graphicsQueue, device, physicalDevice);
  createIndexBuffer(indexBufferMemory, indexBuffer, indices, commandPool, graphicsQueue, device, physicalDevice);
  createUniformBuffers(uniformBuffers, uniformBuffersMemory, uniformBuffersMapped, device, physicalDevice);
  createDescriptorSets(descriptorSets, uniformBuffers, textureImageView, textureSampler, descriptorPool, descriptorSetLayout, device);
}

void Renderer::drawFrame()
{
  waitForFence(inFlightFences[currentFrame], device);

  uint32_t imageIndex;
  VkResult result = acquireNextImageIndex(imageIndex, imageAvailableSemaphores[currentFrame], swapChainObjects.swapChain, device);
  if (result == VK_ERROR_OUT_OF_DATE_KHR)
  {
    recreateSwapChain(commandPool, graphicsQueue, renderPass, swapChainObjects, device, physicalDevice, surface, window);
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
    recreateSwapChain(commandPool, graphicsQueue, renderPass, swapChainObjects, device, physicalDevice, surface, window);
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
}

void Renderer::drawObjects()
{
  updateUniformBuffer(uniformBuffersMapped, currentFrame, swapChainObjects.swapChainExtent);

  bindGraphicsPipeline(commandBuffers[currentFrame], pipeline);

  VkViewport viewport = makeViewport(swapChainObjects.swapChainExtent);
  setViewport(commandBuffers[currentFrame], viewport);

  VkRect2D scissor = makeScissor(swapChainObjects.swapChainExtent);
  setScissor(commandBuffers[currentFrame], scissor);

  bindVertexBuffer(vertexBuffer, commandBuffers[currentFrame]);
  bindIndexBuffer(indexBuffer, commandBuffers[currentFrame]);
  bindDescriptorSets(descriptorSets[currentFrame], commandBuffers[currentFrame], pipelineLayout);
  drawIndexed(commandBuffers[currentFrame], indices.size());
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

  destroyBuffer(vertexBufferMemory, vertexBuffer, device);
  destroyBuffer(indexBufferMemory, indexBuffer, device);

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

  destroyTextureSampler(textureSampler, device);
  destroyImageView(textureImageView, device);
  destroyTextureImage(textureImage, textureImageMemory, device);
  destroyUniformBuffers(uniformBuffers, uniformBuffersMemory, device);
  destroyDescriptorPool(descriptorPool, device);
  destroyDescriptorSetLayout(descriptorSetLayout, device);
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