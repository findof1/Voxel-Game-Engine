#pragma once
#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "vulkanSwapchain.hpp"

VkPipeline createGraphicsPipeline(VkPipelineLayout pipelineLayout, VkRenderPass renderPass, const SwapChainObjects &swapChainObjects, VkDevice device, const std::string &vertexShaderPath, const std::string &fragmentShaderPath, const VkVertexInputBindingDescription *bindingDescription, const std::vector<VkVertexInputAttributeDescription> &attributeDescriptions);
void destroyPipeline(VkPipeline pipeline, VkDevice device);

void bindGraphicsPipeline(VkCommandBuffer commandBuffer, VkPipeline pipeline);

VkShaderModule createShaderModule(VkDevice device, const std::vector<char> &code);
void destroyShaderModule(VkShaderModule shaderModule, VkDevice device);

VkPipelineLayout createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout, VkDevice device);
void destroyPipelineLayout(VkPipelineLayout pipelineLayout, VkDevice device);

VkPipelineShaderStageCreateInfo createShaderStageInfo(VkShaderModule shaderModule, VkShaderStageFlagBits stage);
VkPipelineDynamicStateCreateInfo createDynamicStateInfo();
VkPipelineVertexInputStateCreateInfo createVertexInputStateInfo(const VkVertexInputBindingDescription *bindingDescription, const std::vector<VkVertexInputAttributeDescription> &attributeDescriptions);
VkPipelineInputAssemblyStateCreateInfo createInputAssembleInfo(VkPrimitiveTopology topologyType = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
VkPipelineViewportStateCreateInfo createViewportStateInfo(const SwapChainObjects &swapChainObjects);                // use swapchian to make viewport fullscreen
VkPipelineViewportStateCreateInfo createViewportStateInfo(VkViewport viewport, VkRect2D scissor);                   // used for viewports of any scale
VkPipelineRasterizationStateCreateInfo createRasterizationStateInfo(VkPolygonMode drawMode = VK_POLYGON_MODE_FILL); // note: any mode other than fill required a special gpu feature to be enabled
VkPipelineMultisampleStateCreateInfo createMultisampleStateInfo();                                                  // default to disabled for now
VkPipelineDepthStencilStateCreateInfo createDepthStencilStateInfo();
VkPipelineColorBlendStateCreateInfo createColorBlendStateInfo(const VkPipelineColorBlendAttachmentState *colorBlendAttachment);
VkPipelineColorBlendAttachmentState createColorBlendAttachmentInfo(VkBool32 enableBlend); // have false for now until we add semi-transparent objects
