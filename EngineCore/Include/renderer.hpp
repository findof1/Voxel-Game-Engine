#pragma once

#include <vector>
#include <unordered_map>

#include "vulkanInit.hpp"
#include "vulkanSurface.hpp"
#include "vulkanDevice.hpp"
#include "vulkanSwapChain.hpp"
#include "vulkanRenderPass.hpp"
#include "vulkanPipeline.hpp"
#include "vulkanFramebuffers.hpp"
#include "vulkanSyncObjects.hpp"
#include "vulkanCommandBuffer.hpp"
#include "vulkanDrawing.hpp"
#include "vulkanBufferObjects.hpp"
#include "vulkanBufferUtils.hpp"
#include "vulkanDescriptors.hpp"
#include "vulkanImages.hpp"

#include "texture.hpp"

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

const std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR};

const int MAX_FRAMES_IN_FLIGHT = 2;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Renderer
{
public:
  GLFWwindow *window;
  VkInstance instance;
  VkSurfaceKHR surface;
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  SwapChainObjects swapChainObjects;
  VkRenderPass renderPass;
  VkDescriptorSetLayout descriptorSetLayout;
  VkDescriptorPool descriptorPool;
  VkPipelineLayout pipelineLayout;
  VkPipeline pipeline;
  VkCommandPool commandPool;
  std::vector<VkCommandBuffer> commandBuffers;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;

  std::unordered_map<std::string, Texture> textures;

  // counts between 0 and MAX_FRAMES_IN_FLIGHT and then resets to 0 using (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT at the end of each frame
  uint32_t currentFrame = 0;
  bool framebufferResized = false;

  Texture createTexutre(const std::string &name, const std::string &filePath);
  Texture createTexutreArray(const std::string &name, const std::vector<std::string> filePaths);
  Texture getTexture(const std::string &name);

  Renderer(GLFWwindow *window);
  void init();
  void startFrame();
  void endFrame();
  void startRendering(uint32_t imageIndex);
  void endRendering();
  void cleanup();

private:
  uint32_t imageIndex;
};