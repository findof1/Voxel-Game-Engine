#pragma once
#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

// note: descriptors make me want to cry sometimes

VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device);
void destroyDescriptorSetLayout(VkDescriptorSetLayout descriptorLayout, VkDevice device);

VkDescriptorPool createDescriptorPool(VkDevice device);
void destroyDescriptorPool(VkDescriptorPool descriptorPool, VkDevice device);
void createDescriptorSets(std::vector<VkDescriptorSet> &descriptorSets, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorLayout, VkDevice device);