#include "vulkanPipeline.hpp"
#include "fileUtils.hpp"
#include "renderer.hpp"
#include "vertexData.hpp"

VkPipeline createGraphicsPipeline(VkPipelineLayout pipelineLayout, VkRenderPass renderPass, const SwapChainObjects &swapChainObjects, VkDevice device, const std::string &vertexShaderPath, const std::string &fragmentShaderPath, const VkVertexInputBindingDescription *bindingDescription, const std::vector<VkVertexInputAttributeDescription> &attributeDescriptions)
{
  std::vector<char> vertShaderCode = readFile(vertexShaderPath);
  std::vector<char> fragShaderCode = readFile(fragmentShaderPath);

  VkShaderModule vertShaderModule = createShaderModule(device, vertShaderCode);
  VkShaderModule fragShaderModule = createShaderModule(device, fragShaderCode);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo = createShaderStageInfo(vertShaderModule, VK_SHADER_STAGE_VERTEX_BIT);
  VkPipelineShaderStageCreateInfo fragShaderStageInfo = createShaderStageInfo(fragShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT);
  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

  VkPipelineDynamicStateCreateInfo dynamicState = createDynamicStateInfo();

  VkPipelineVertexInputStateCreateInfo vertexInputInfo = createVertexInputStateInfo(bindingDescription, attributeDescriptions);
  VkPipelineInputAssemblyStateCreateInfo inputAssembly = createInputAssembleInfo();
  VkPipelineViewportStateCreateInfo viewportState = createViewportStateInfo(swapChainObjects);
  VkPipelineRasterizationStateCreateInfo rasterizer = createRasterizationStateInfo();
  VkPipelineMultisampleStateCreateInfo multisampling = createMultisampleStateInfo();
  VkPipelineDepthStencilStateCreateInfo depthStencil = createDepthStencilStateInfo();
  VkPipelineColorBlendAttachmentState colorBlendAttachment = createColorBlendAttachmentInfo(VK_FALSE);
  VkPipelineColorBlendStateCreateInfo colorBlending = createColorBlendStateInfo(&colorBlendAttachment);

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = pipelineLayout;
  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = -1;

  VkPipeline graphicsPipeline;
  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
  {
    std::cerr << "Failed to create graphics pipeline!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }

  destroyShaderModule(vertShaderModule, device);
  destroyShaderModule(fragShaderModule, device);
  return graphicsPipeline;
}

void destroyPipeline(VkPipeline pipeline, VkDevice device)
{
  if (pipeline != VK_NULL_HANDLE)
  {
    vkDestroyPipeline(device, pipeline, nullptr);
  }
}

void bindGraphicsPipeline(VkCommandBuffer commandBuffer, VkPipeline pipeline)
{
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

VkPipelineLayout createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout, VkDevice device)
{
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

  // no push constants for now, but they will be useful later
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  VkPipelineLayout pipelineLayout;
  if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
  {
    std::cerr << "Failed to create pipeline layout!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }
  return pipelineLayout;
}

void destroyPipelineLayout(VkPipelineLayout pipelineLayout, VkDevice device)
{
  if (pipelineLayout != VK_NULL_HANDLE)
  {
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
  }
}

VkShaderModule createShaderModule(VkDevice device, const std::vector<char> &code)
{
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
  {
    std::cerr << "Failed to create shader module!" << std::endl;
    glfwTerminate();
    std::cerr << "Press Enter to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
  }

  return shaderModule;
}

void destroyShaderModule(VkShaderModule shaderModule, VkDevice device)
{
  if (shaderModule != VK_NULL_HANDLE)
  {
    vkDestroyShaderModule(device, shaderModule, nullptr);
  }
}

VkPipelineShaderStageCreateInfo createShaderStageInfo(VkShaderModule shaderModule, VkShaderStageFlagBits stage)
{
  VkPipelineShaderStageCreateInfo shaderStageInfo{};
  shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStageInfo.stage = stage;
  shaderStageInfo.module = shaderModule;
  shaderStageInfo.pName = "main"; // note: this has some cool usage to come back to later on

  return shaderStageInfo;
}

VkPipelineDynamicStateCreateInfo createDynamicStateInfo()
{
  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();
  return dynamicState;
}

VkPipelineVertexInputStateCreateInfo createVertexInputStateInfo(const VkVertexInputBindingDescription *bindingDescription, const std::vector<VkVertexInputAttributeDescription> &attributeDescriptions)
{
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
  vertexInputInfo.pVertexBindingDescriptions = bindingDescription;
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

  return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo createInputAssembleInfo(VkPrimitiveTopology topologyType)
{
  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = topologyType;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  return inputAssembly;
}

VkPipelineViewportStateCreateInfo createViewportStateInfo(const SwapChainObjects &swapChainObjects)
{
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)swapChainObjects.swapChainExtent.width;
  viewport.height = (float)swapChainObjects.swapChainExtent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swapChainObjects.swapChainExtent;

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  return viewportState;
}

VkPipelineViewportStateCreateInfo createViewportStateInfo(VkViewport viewport, VkRect2D scissor)
{
  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  return viewportState;
}

VkPipelineRasterizationStateCreateInfo createRasterizationStateInfo(VkPolygonMode drawMode)
{
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE; // disables output to framebuffer if true
  rasterizer.polygonMode = drawMode;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f;
  rasterizer.depthBiasClamp = 0.0f;
  rasterizer.depthBiasSlopeFactor = 0.0f;

  return rasterizer;
}

VkPipelineMultisampleStateCreateInfo createMultisampleStateInfo()
{
  // currently disabled, so default info

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;
  multisampling.pSampleMask = nullptr;
  multisampling.alphaToCoverageEnable = VK_FALSE;
  multisampling.alphaToOneEnable = VK_FALSE;

  return multisampling;
}

VkPipelineDepthStencilStateCreateInfo createDepthStencilStateInfo()
{
  VkPipelineDepthStencilStateCreateInfo depthStencil{};
  depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.minDepthBounds = 0.0f;
  depthStencil.maxDepthBounds = 1.0f;
  depthStencil.stencilTestEnable = VK_FALSE;
  depthStencil.front = {};
  depthStencil.back = {};

  return depthStencil;
}

VkPipelineColorBlendStateCreateInfo createColorBlendStateInfo(const VkPipelineColorBlendAttachmentState *colorBlendAttachment)
{
  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  return colorBlending;
}

VkPipelineColorBlendAttachmentState createColorBlendAttachmentInfo(VkBool32 enableBlend)
{
  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = enableBlend;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  return colorBlendAttachment;
}