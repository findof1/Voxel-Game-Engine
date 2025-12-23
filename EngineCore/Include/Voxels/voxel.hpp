#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>

struct BlockType
{
  std::string name;
  bool visible = true;
  // bool solid;
  // bool transparent;
  // bool emissive;

  // uint8_t emission;

  int textureTop;
  int textureBottom;
  int textureSide;

  // CollisionType collision;
};

struct BlockRegistry
{
  std::vector<BlockType> blocks;
  std::unordered_map<std::string, uint32_t> nameToId;
};

struct Voxel
{
  uint32_t type;
};