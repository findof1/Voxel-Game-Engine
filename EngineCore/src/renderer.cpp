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
  renderPass = createRenderPass(swapChainObjects, device, physicalDevice);

  std::array<VkDescriptorSetLayoutBinding, 2> bindings{
      uniformBufferBinding(0, VK_SHADER_STAGE_VERTEX_BIT),
      combinedImageSamplerBinding(1, VK_SHADER_STAGE_FRAGMENT_BIT)};

  descriptorSetLayout = createDescriptorSetLayout(device, bindings);

  pipelineLayout = createPipelineLayout(descriptorSetLayout, device);

  auto vertexBinding = Vertex::getBindingDescription();
  auto vertexAttributes = Vertex::getAttributeDescriptions();

  pipeline = createGraphicsPipeline(pipelineLayout, renderPass, swapChainObjects, device, "shaders/vert.spv", "shaders/frag.spv", &vertexBinding, vertexAttributes);

  auto voxelVertexBinding = VoxelVertex::getBindingDescription();
  auto voxelVertexAttributes = VoxelVertex::getAttributeDescriptions();

  voxelPipeline = createGraphicsPipeline(pipelineLayout, renderPass, swapChainObjects, device, "shaders/voxelVert.spv", "shaders/voxelFrag.spv", &voxelVertexBinding, voxelVertexAttributes);

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
}

Texture Renderer::createTexutre(const std::string &name, const std::string &filePath)
{
  Texture texture;
  createTextureImage(texture.image, texture.memory, filePath, commandPool, graphicsQueue, device, physicalDevice);
  texture.view = createImageView(texture.image, VK_FORMAT_R8G8B8A8_SRGB, device, VK_IMAGE_ASPECT_COLOR_BIT);
  texture.sampler = createTextureSampler(device, physicalDevice);

  textures.emplace(name, texture);
  return texture;
}

Texture Renderer::createTexutreArray(const std::string &name, const std::vector<std::string> filePaths)
{
  Texture texture;
  createTextureArrayImage(texture.image, texture.memory, filePaths, commandPool, graphicsQueue, device, physicalDevice);
  texture.view = createImageView(texture.image, VK_FORMAT_R8G8B8A8_SRGB, device, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D_ARRAY, filePaths.size());
  texture.sampler = createTextureSampler(device, physicalDevice);

  textures.emplace(name, texture);
  return texture;
}

Texture Renderer::getTexture(const std::string &name)
{
  if (textures.find(name) == textures.end())
  {
    std::cerr << "getTexture failed with texture name: " << name << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }
  return textures.at(name);
}

void Renderer::startFrame()
{
  waitForFence(inFlightFences[currentFrame], device);

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
}

void Renderer::endFrame()
{
  endRendering();

  submitFrame(imageAvailableSemaphores[currentFrame], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, renderFinishedSemaphores[imageIndex], inFlightFences[currentFrame], commandBuffers[currentFrame], graphicsQueue);

  VkResult result = presentFrame(imageIndex, renderFinishedSemaphores[imageIndex], swapChainObjects.swapChain, presentQueue);

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

void Renderer::endRendering()
{
  endRenderPass(commandBuffers[currentFrame]);
  endCommandBuffer(commandBuffers[currentFrame]);
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

  for (auto [_, tex] : textures)
  {
    destroyTextureSampler(tex.sampler, device);
    destroyImageView(tex.view, device);
    destroyTextureImage(tex.image, tex.memory, device);
  }

  destroyDescriptorPool(descriptorPool, device);
  destroyDescriptorSetLayout(descriptorSetLayout, device);
  destroyCommandPool(commandPool, device);
  destroyPipeline(pipeline, device);
  destroyPipeline(voxelPipeline, device);
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