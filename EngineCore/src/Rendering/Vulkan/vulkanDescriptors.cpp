#include "vulkanDescriptors.hpp"
#include "renderer.hpp"
#include "uniformData.hpp"

VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device, std::span<const VkDescriptorSetLayoutBinding> bindings)
{
  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  VkDescriptorSetLayout layout;
  if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout) != VK_SUCCESS)
  {
    std::cerr << "Failed to create descriptor set layout!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }

  return layout;
}

VkDescriptorSetLayoutBinding storageBufferBinding(uint32_t binding, VkShaderStageFlags stages)
{
  VkDescriptorSetLayoutBinding b{};
  b.binding = binding;
  b.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  b.descriptorCount = 1;
  b.stageFlags = stages;
  b.pImmutableSamplers = nullptr;
  return b;
}

VkDescriptorSetLayoutBinding uniformBufferBinding(uint32_t binding, VkShaderStageFlags stages)
{
  VkDescriptorSetLayoutBinding b{};
  b.binding = binding;
  b.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  b.descriptorCount = 1;
  b.stageFlags = stages;
  b.pImmutableSamplers = nullptr;
  return b;
}

VkDescriptorSetLayoutBinding combinedImageSamplerBinding(uint32_t binding, VkShaderStageFlags stages)
{
  VkDescriptorSetLayoutBinding b{};
  b.binding = binding;
  b.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  b.descriptorCount = 1;
  b.stageFlags = stages;
  b.pImmutableSamplers = nullptr;
  return b;
}

void destroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout, VkDevice device)
{
  if (descriptorSetLayout != VK_NULL_HANDLE)
  {
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
  }
}

VkDescriptorPool createDescriptorPool(VkDevice device)
{
  std::array<VkDescriptorPoolSize, 3> poolSizes{};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = 256;
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = 256;
  poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  poolSizes[2].descriptorCount = 256;

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = 10000;
  poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

  VkDescriptorPool descriptorPool;

  if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
  {
    std::cerr << "Failed to create descriptor pool!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }

  return descriptorPool;
}

void destroyDescriptorPool(VkDescriptorPool descriptorPool, VkDevice device)
{
  if (descriptorPool != VK_NULL_HANDLE)
  {
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
  }
}

VkWriteDescriptorSet writeStorageBuffer(VkDescriptorSet dstSet, uint32_t binding, const VkDescriptorBufferInfo *bufferInfo)
{
  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.dstSet = dstSet;
  write.dstBinding = binding;
  write.dstArrayElement = 0;
  write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  write.descriptorCount = 1;
  write.pBufferInfo = bufferInfo;
  return write;
}

VkWriteDescriptorSet writeUniformBuffer(VkDescriptorSet dstSet, uint32_t binding, const VkDescriptorBufferInfo *bufferInfo)
{
  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.dstSet = dstSet;
  write.dstBinding = binding;
  write.dstArrayElement = 0;
  write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  write.descriptorCount = 1;
  write.pBufferInfo = bufferInfo;
  return write;
}

VkWriteDescriptorSet writeCombinedImageSampler(VkDescriptorSet dstSet, uint32_t binding, const VkDescriptorImageInfo *imageInfo)
{
  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.dstSet = dstSet;
  write.dstBinding = binding;
  write.dstArrayElement = 0;
  write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  write.descriptorCount = 1;
  write.pImageInfo = imageInfo;
  return write;
}

void allocateDescriptorSets(std::vector<VkDescriptorSet> &descriptorSets, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout, VkDevice device, int count)
{
  std::vector<VkDescriptorSetLayout> layouts(count, descriptorSetLayout);
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(count);
  allocInfo.pSetLayouts = layouts.data();
  descriptorSets.resize(count);
  if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
  {
    std::cerr << "Failed to allocate descriptor sets!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }
}

void allocateDescriptorSet(VkDescriptorSet *descriptorSet, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout, VkDevice device)
{
  std::vector<VkDescriptorSetLayout> layouts(1, descriptorSetLayout);
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(1);
  allocInfo.pSetLayouts = layouts.data();
  if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSet) != VK_SUCCESS)
  {
    std::cerr << "Failed to allocate descriptor sets!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }
}

void updateDescriptorSets(VkDevice device, std::span<const VkWriteDescriptorSet> descriptorWrites)
{
  vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void bindDescriptorSet(VkDescriptorSet descriptorSet, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int firstSet, int setCount)
{
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
}

void bindDescriptorSets(std::vector<VkDescriptorSet> &descriptorSets, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, descriptorSets.size(), descriptorSets.data(), 0, nullptr);
}