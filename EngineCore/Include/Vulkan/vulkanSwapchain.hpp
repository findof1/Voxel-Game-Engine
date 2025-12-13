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
};

SwapChainObjects createSwapChain(VkSurfaceKHR surface, VkDevice device, VkPhysicalDevice physicalDevice);
void destroySwapChain(VkSwapchainKHR swapChain, VkDevice device);

void createImageViews(SwapChainObjects &swapChainObjects);
void destroyImageViews(std::vector<VkImageView> &swapChainImageViews, VkDevice device);

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice);
VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);