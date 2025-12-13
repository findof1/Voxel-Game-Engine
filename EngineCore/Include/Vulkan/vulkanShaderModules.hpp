#pragma once
#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

VkShaderModule createShaderModule(VkDevice device, const std::vector<char> &code);
void destroyShaderModule(VkShaderModule shaderModule, VkDevice device);