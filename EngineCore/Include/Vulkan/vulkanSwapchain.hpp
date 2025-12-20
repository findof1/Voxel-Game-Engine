#pragma once
#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

struct SwapChainSupportDetails
{
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct SwapChainObjects
{
  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;

  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;

  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;
};

SwapChainObjects createSwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow *window);
void recreateSwapChain(VkCommandPool commandPool, VkQueue graphicsQueue, VkRenderPass renderPass, SwapChainObjects &swapChainObjects, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow *window);
void cleanupSwapChain(SwapChainObjects &swapChainObjects, VkDevice device);
void destroySwapChain(VkSwapchainKHR swapChain, VkDevice device);

VkResult acquireNextImageIndex(uint32_t &imageIndex, VkSemaphore imageAvailableSemaphore, VkSwapchainKHR swapChain, VkDevice device);

void createImageViews(SwapChainObjects &swapChainObjects, VkDevice device);
void destroyImageViews(std::vector<VkImageView> &swapChainImageViews, VkDevice device);

SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice);
VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
VkExtent2D chooseSwapExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR &capabilities);

void createDepthResources(SwapChainObjects &swapChainObjects, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice);

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);
bool hasStencilComponent(VkFormat format);
VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice physicalDevice);