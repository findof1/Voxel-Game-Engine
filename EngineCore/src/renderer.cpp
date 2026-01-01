#include "renderer.hpp"
#include "uniformData.hpp"
#include "voxelSystem.hpp"

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

  std::array<VkDescriptorSetLayoutBinding, 1> cameraBindings{
      uniformBufferBinding(0, VK_SHADER_STAGE_VERTEX_BIT)};

  cameraSetLayout = createDescriptorSetLayout(device, cameraBindings);

  std::array<VkDescriptorSetLayoutBinding, 1> imageBindings{
      combinedImageSamplerBinding(0, VK_SHADER_STAGE_FRAGMENT_BIT)};

  imageSetLayout = createDescriptorSetLayout(device, imageBindings);

  std::array<VkDescriptorSetLayoutBinding, 1> voxelBindings{
      storageBufferBinding(0, VK_SHADER_STAGE_VERTEX_BIT)};

  voxelSetLayout = createDescriptorSetLayout(device, voxelBindings);

  VkPushConstantRange pushConstantRanges = createPushConstantInfo(sizeof(PushConstants), VK_SHADER_STAGE_VERTEX_BIT);
  std::vector<VkDescriptorSetLayout> setLayouts = {cameraSetLayout, imageSetLayout};
  pipelineLayout = createPipelineLayout(setLayouts, device, &pushConstantRanges);
  auto vertexBinding = Vertex::getBindingDescription();
  auto vertexAttributes = Vertex::getAttributeDescriptions();

  pipeline = createGraphicsPipeline(pipelineLayout, renderPass, swapChainObjects, device, "shaders/vert.spv", "shaders/frag.spv", &vertexBinding, vertexAttributes);

  VkPushConstantRange voxelPushConstantRanges = createPushConstantInfo(sizeof(VoxelPushConstants), VK_SHADER_STAGE_VERTEX_BIT);
  std::vector<VkDescriptorSetLayout> voxelSetLayouts = {cameraSetLayout, voxelSetLayout, imageSetLayout};
  voxelPipelineLayout = createPipelineLayout(voxelSetLayouts, device, &voxelPushConstantRanges);
  auto voxelVertexBinding = VoxelVertex::getBindingDescription();
  auto voxelVertexAttributes = VoxelVertex::getAttributeDescriptions();

  voxelPipeline = createGraphicsPipeline(voxelPipelineLayout, renderPass, swapChainObjects, device, "shaders/voxelVert.spv", "shaders/voxelFrag.spv", &voxelVertexBinding, voxelVertexAttributes);

  commandPool = createCommandPool(device, physicalDevice, surface, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
  commandBuffers = createCommandBuffers(commandPool, device, MAX_FRAMES_IN_FLIGHT);
  createDepthResources(swapChainObjects, commandPool, graphicsQueue, device, physicalDevice);
  createSwapchainFramebuffers(renderPass, swapChainObjects, device);
  descriptorPool = createDescriptorPool(device);

  VkDeviceSize storageBufferSize = sizeof(ShaderBufferObject) * MAX_CHUNKS;
  createStorageBuffer(storageBufferSize, storageBuffer, storageBufferMemory, storageBufferMapped, device, physicalDevice);
  storageBufferAccess = static_cast<ShaderBufferObject *>(storageBufferMapped);
  createUniformBuffers(uniformBuffers, uniformBuffersMemory, uniformBuffersMapped, device, physicalDevice);
  createDescriptorSets();

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

void Renderer::createDescriptorSets()
{
  // camera descriptors
  allocateDescriptorSets(cameraSets, descriptorPool, cameraSetLayout, device, MAX_FRAMES_IN_FLIGHT);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
  {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBuffers[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    std::array<VkWriteDescriptorSet, 1> descriptorWrites{
        writeUniformBuffer(cameraSets[i], 0, &bufferInfo)};

    updateDescriptorSets(device, descriptorWrites);
  }

  // voxel descriptors
  allocateDescriptorSet(&voxelSet, descriptorPool, voxelSetLayout, device);
  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = storageBuffer;
  bufferInfo.offset = 0;
  bufferInfo.range = sizeof(ShaderBufferObject) * MAX_CHUNKS;

  std::array<VkWriteDescriptorSet, 1> descriptorWrites{
      writeStorageBuffer(voxelSet, 0, &bufferInfo)};

  updateDescriptorSets(device, descriptorWrites);
}

Texture Renderer::createTexutre(const std::string &name, const std::string &filePath)
{
  Texture texture;
  createTextureImage(texture.image, texture.memory, filePath, commandPool, graphicsQueue, device, physicalDevice);
  texture.view = createImageView(texture.image, VK_FORMAT_R8G8B8A8_SRGB, device, VK_IMAGE_ASPECT_COLOR_BIT);
  texture.sampler = createTextureSampler(device, physicalDevice);

  textures.emplace(name, texture);
  VkDescriptorSet &imageSet = textures.at(name).imageSet;
  // create descriptor set for the texture
  allocateDescriptorSet(&imageSet, descriptorPool, imageSetLayout, device);

  VkDescriptorImageInfo imageInfo{};
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  imageInfo.imageView = textures.at(name).view;
  imageInfo.sampler = textures.at(name).sampler;

  std::array<VkWriteDescriptorSet, 1> descriptorWrites{
      writeCombinedImageSampler(imageSet, 0, &imageInfo)};

  updateDescriptorSets(device, descriptorWrites);
  //

  return textures.at(name);
}

Texture Renderer::createTexutreArray(const std::string &name, const std::vector<std::string> filePaths)
{
  Texture texture;
  createTextureArrayImage(texture.image, texture.memory, filePaths, commandPool, graphicsQueue, device, physicalDevice);
  texture.view = createImageView(texture.image, VK_FORMAT_R8G8B8A8_SRGB, device, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D_ARRAY, filePaths.size());
  texture.sampler = createTextureSampler(device, physicalDevice);

  // create descriptor set for the texture
  allocateDescriptorSet(&texture.imageSet, descriptorPool, imageSetLayout, device);

  VkDescriptorImageInfo imageInfo{};
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  imageInfo.imageView = texture.view;
  imageInfo.sampler = texture.sampler;

  std::array<VkWriteDescriptorSet, 1> descriptorWrites{
      writeCombinedImageSampler(texture.imageSet, 0, &imageInfo)};

  updateDescriptorSets(device, descriptorWrites);
  //

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

Texture *Renderer::getTexturePointer(const std::string &name)
{
  if (textures.find(name) == textures.end())
  {
    std::cerr << "getTexture failed with texture name: " << name << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }
  return &textures.at(name);
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

  destroyUniformBuffers(uniformBuffers, uniformBuffersMemory, device);
  uniformBuffers.clear();
  uniformBuffersMemory.clear();
  uniformBuffersMapped.clear();

  destroyStorageBuffer(storageBuffer, storageBufferMemory, device);

  vkFreeDescriptorSets(device, descriptorPool, static_cast<uint32_t>(cameraSets.size()), cameraSets.data());
  vkFreeDescriptorSets(device, descriptorPool, 1, &voxelSet);

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
    vkFreeDescriptorSets(device, descriptorPool, 1, &tex.imageSet);
  }

  destroyDescriptorPool(descriptorPool, device);
  destroyDescriptorSetLayout(cameraSetLayout, device);
  destroyDescriptorSetLayout(imageSetLayout, device);
  destroyDescriptorSetLayout(voxelSetLayout, device);
  destroyCommandPool(commandPool, device);
  destroyPipeline(pipeline, device);
  destroyPipelineLayout(pipelineLayout, device);
  destroyPipeline(voxelPipeline, device);
  destroyPipelineLayout(voxelPipelineLayout, device);
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