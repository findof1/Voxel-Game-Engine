#pragma once
#include <iostream>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

void createTextureImage(VkImage &textureImage, VkDeviceMemory &textureImageMemory, const std::string &filePath, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device, VkPhysicalDevice physicalDevice);
void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory, VkDevice device, VkPhysicalDevice physicalDevice);

void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device);

void destroyTextureImage(VkImage textureImage, VkDeviceMemory textureImageMemory, VkDevice device);

VkImageView createImageView(VkImage image, VkFormat format, VkDevice device);
void destroyImageView(VkImageView imageView, VkDevice device);

VkSampler createTextureSampler(VkDevice device, VkPhysicalDevice physicalDevice);
void destroyTextureSampler(VkSampler sampler, VkDevice device);