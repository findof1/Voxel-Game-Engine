#pragma once

#include <vulkan/vulkan.h>

struct Texture
{
  VkImage image;
  VkDeviceMemory memory;
  VkImageView view;
  VkSampler sampler;
};