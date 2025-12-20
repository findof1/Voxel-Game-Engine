#pragma once
#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

// note: descriptors make me want to cry sometimes

VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device);
void destroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout, VkDevice device);

VkDescriptorPool createDescriptorPool(VkDevice device);
void destroyDescriptorPool(VkDescriptorPool descriptorPool, VkDevice device);
void createDescriptorSets(std::vector<VkDescriptorSet> &descriptorSets, std::vector<VkBuffer> &uniformBuffers, VkImageView textureImageView, VkSampler textureSampler, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout, VkDevice device);

void bindDescriptorSets(VkDescriptorSet descriptorSet, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);