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
  std::array<VkDescriptorPoolSize, 2> poolSizes{};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = 10000;
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = 10000;
  // poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  // poolSizes[2].descriptorCount = 5000;

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

void createDescriptorSets(std::vector<VkDescriptorSet> &descriptorSets, std::vector<VkBuffer> &uniformBuffers, VkImageView textureImageView, VkSampler textureSampler, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout, VkDevice device)
{
  allocateDescriptorSets(descriptorSets, descriptorPool, descriptorSetLayout, device, MAX_FRAMES_IN_FLIGHT);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
  {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = uniformBuffers[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureImageView;
    imageInfo.sampler = textureSampler;

    std::array<VkWriteDescriptorSet, 2> descriptorWrites{
        writeUniformBuffer(descriptorSets[i], 0, &bufferInfo),
        writeCombinedImageSampler(descriptorSets[i], 1, &imageInfo)};

    updateDescriptorSets(device, descriptorWrites);
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

void updateDescriptorSets(VkDevice device, std::span<const VkWriteDescriptorSet> descriptorWrites)
{
  vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void bindDescriptorSets(VkDescriptorSet descriptorSet, VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
}