#pragma once
#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

void createTextureArrayImage(VkImage &textureImage, VkDeviceMemory &textureImageMemory, const std::vector<std::string> &filePaths, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice);

void createTextureImage(VkImage &textureImage, VkDeviceMemory &textureImageMemory, const std::string &filePath, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice);
void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory, VkDevice device, VkPhysicalDevice physicalDevice, int layers = 1);

void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, int layers = 1);

void destroyTextureImage(VkImage textureImage, VkDeviceMemory textureImageMemory, VkDevice device);

VkImageView createImageView(VkImage image, VkFormat format, VkDevice device, VkImageAspectFlags aspectFlags, VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, int layerCount = 1);
void destroyImageView(VkImageView imageView, VkDevice device);

VkSampler createTextureSampler(VkDevice device, VkPhysicalDevice physicalDevice);
void destroyTextureSampler(VkSampler sampler, VkDevice device);
