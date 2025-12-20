#include <algorithm>
#include <cstdint>
#include <limits>

#include "vulkanSwapchain.hpp"
#include "vulkanDevice.hpp"
#include "vulkanFramebuffers.hpp"
#include "vulkanImages.hpp"

SwapChainObjects createSwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow *window)
{
  SwapChainObjects swapChainObjects;

  SwapChainSupportDetails swapChainSupport = querySwapChainSupport(surface, physicalDevice);

  VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = chooseSwapExtent(window, swapChainSupport.capabilities);

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

  if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
  {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices = findQueueFamilies(surface, physicalDevice);
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

  if (indices.graphicsFamily != indices.presentFamily)
  {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  }
  else
  {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }
  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // interesting field because you can make semi-transparent windows using a different setting (I think)
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChainObjects.swapChain) != VK_SUCCESS)
  {
    std::cerr << "Failed to create swap chain!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }

  vkGetSwapchainImagesKHR(device, swapChainObjects.swapChain, &imageCount, nullptr);
  swapChainObjects.swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device, swapChainObjects.swapChain, &imageCount, swapChainObjects.swapChainImages.data());

  swapChainObjects.swapChainImageFormat = surfaceFormat.format;
  swapChainObjects.swapChainExtent = extent;

  return swapChainObjects;
}

void recreateSwapChain(VkCommandPool commandPool, VkQueue graphicsQueue, VkRenderPass renderPass, SwapChainObjects &swapChainObjects, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow *window)
{
  int width = 0, height = 0;
  glfwGetFramebufferSize(window, &width, &height);
  while (width == 0 || height == 0)
  {
    glfwGetFramebufferSize(window, &width, &height);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(device);

  cleanupSwapChain(swapChainObjects, device);

  SwapChainObjects newSwapChainObjects = createSwapChain(device, physicalDevice, surface, window);
  createImageViews(newSwapChainObjects, device);
  createDepthResources(swapChainObjects, commandPool, graphicsQueue, device, physicalDevice);
  createSwapchainFramebuffers(renderPass, newSwapChainObjects, device);

  swapChainObjects = newSwapChainObjects;
}

void cleanupSwapChain(SwapChainObjects &swapChainObjects, VkDevice device)
{
  vkDestroyImageView(device, swapChainObjects.depthImageView, nullptr);
  vkDestroyImage(device, swapChainObjects.depthImage, nullptr);
  vkFreeMemory(device, swapChainObjects.depthImageMemory, nullptr);

  destroySwapchainFramebuffers(swapChainObjects, device);
  destroyImageViews(swapChainObjects.swapChainImageViews, device);
  destroySwapChain(swapChainObjects.swapChain, device);
}

void destroySwapChain(VkSwapchainKHR swapChain, VkDevice device)
{
  if (swapChain != VK_NULL_HANDLE)
  {
    vkDestroySwapchainKHR(device, swapChain, nullptr);
  }
}

VkResult acquireNextImageIndex(uint32_t &imageIndex, VkSemaphore imageAvailableSemaphore, VkSwapchainKHR swapChain, VkDevice device)
{
  return vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
}

void createImageViews(SwapChainObjects &swapChainObjects, VkDevice device)
{
  swapChainObjects.swapChainImageViews.resize(swapChainObjects.swapChainImages.size());

  for (uint32_t i = 0; i < swapChainObjects.swapChainImages.size(); i++)
  {
    swapChainObjects.swapChainImageViews[i] = createImageView(swapChainObjects.swapChainImages[i], swapChainObjects.swapChainImageFormat, device, VK_IMAGE_ASPECT_COLOR_BIT);
  }
}

void destroyImageViews(std::vector<VkImageView> &swapChainImageViews, VkDevice device)
{
  for (auto imageView : swapChainImageViews)
  {
    destroyImageView(imageView, device);
  }
}

SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice)
{
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

  if (formatCount != 0)
  {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

  if (presentModeCount != 0)
  {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
  }

  return details;
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
  for (const auto &availableFormat : availableFormats)
  {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
    {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
  for (const auto &availablePresentMode : availablePresentModes)
  {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
    {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR &capabilities)
{
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
  {
    return capabilities.currentExtent;
  }
  else
  {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actualExtent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)};

    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

void createDepthResources(SwapChainObjects &swapChainObjects, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice)
{
  VkFormat depthFormat = findDepthFormat(physicalDevice);

  createImage(swapChainObjects.swapChainExtent.width, swapChainObjects.swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, swapChainObjects.depthImage, swapChainObjects.depthImageMemory, device, physicalDevice);

  swapChainObjects.depthImageView = createImageView(swapChainObjects.depthImage, depthFormat, device, VK_IMAGE_ASPECT_DEPTH_BIT);

  transitionImageLayout(swapChainObjects.depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, commandPool, graphicsQueue, device);
}

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice)
{
  return findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, physicalDevice);
}

bool hasStencilComponent(VkFormat format)
{
  return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice physicalDevice)
{
  for (VkFormat format : candidates)
  {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
    {
      return format;
    }
    else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
    {
      return format;
    }
  }

  std::cerr << "Failed to find supported format!" << std::endl;
  glfwTerminate();
  std::cerr << "Press Enter to exit..." << std::endl;
  std::cin.get();
  exit(EXIT_FAILURE);
}