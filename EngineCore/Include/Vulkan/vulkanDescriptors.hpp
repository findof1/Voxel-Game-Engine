#pragma once
#include <iostream>
#include <vector>
#include <span>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

// note: descriptors make me want to cry sometimes

VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device, std::span<const VkDescriptorSetLayoutBinding> bindings);
void destroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout, VkDevice device);

VkDescriptorSetLayoutBinding storageBufferBinding(uint32_t binding, VkShaderStageFlags stages);
VkDescriptorSetLayoutBinding uniformBufferBinding(uint32_t binding, VkShaderStageFlags stages);
VkDescriptorSetLayoutBinding combinedImageSamplerBinding(uint32_t binding, VkShaderStageFlags stages);

VkDescriptorPool createDescriptorPool(VkDevice device);
void destroyDescriptorPool(VkDescriptorPool descriptorPool, VkDevice device);

void createDescriptorSets(std::vector<VkDescriptorSet> &descriptorSets, std::vector<VkBuffer> &uniformBuffers, VkImageView textureImageView, VkSampler textureSampler, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout, VkDevice device);

VkWriteDescriptorSet writeStorageBuffer(VkDescriptorSet dstSet, uint32_t binding, const VkDescriptorBufferInfo *bufferInfo);
VkWriteDescriptorSet writeUniformBuffer(VkDescriptorSet dstSet, uint32_t binding, const VkDescriptorBufferInfo *bufferInfo);
VkWriteDescriptorSet writeCombinedImageSampler(VkDescriptorSet dstSet, uint32_t binding, const VkDescriptorImageInfo *imageInfo);
void allocateDescriptorSets(std::vector<VkDescriptorSet> &descriptorSets, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout, VkDevice device, int count);

void updateDescriptorSets(VkDevice device, std::span<const VkWriteDescriptorSet> descriptorWrites);

void bindDescriptorSets(VkDescriptorSet descriptorSet, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);