#pragma once
#include <iostream>
#include <array>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct Vertex
{
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 texCoord;

  static VkVertexInputBindingDescription getBindingDescription()
  {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }

  static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
  {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
    attributeDescriptions.resize(3);

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    return attributeDescriptions;
  }
};

struct PushConstants
{
  alignas(16) glm::mat4 model;
};

struct VoxelVertex
{
  int16_t pos[3];    // 12.4 fixed point, 12 bytes for int part, 4 bytes for fractional part
  uint16_t texCoord; // u: 4.4 fixed point, v: 4.4 fixed point
  uint16_t texIndex;

  static uint16_t packUVs(const glm::vec2 &uv)
  {
    uint16_t u = uint16_t(glm::clamp(uv.x, 0.0f, 15.9375f) * 16.0f);
    uint16_t v = uint16_t(glm::clamp(uv.y, 0.0f, 15.9375f) * 16.0f);
    return (v << 8) | u;
  }

  static int16_t packPosition(float value)
  {
    int16_t valueFixed = static_cast<int16_t>(value * 16.0f);

    return valueFixed;
  }

  static VkVertexInputBindingDescription getBindingDescription()
  {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(VoxelVertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }

  static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
  {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
    attributeDescriptions.resize(3);

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R16G16B16_SINT;
    attributeDescriptions[0].offset = offsetof(VoxelVertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R16_UINT;
    attributeDescriptions[1].offset = offsetof(VoxelVertex, texCoord);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R16_UINT;
    attributeDescriptions[2].offset = offsetof(VoxelVertex, texIndex);

    return attributeDescriptions;
  }
};