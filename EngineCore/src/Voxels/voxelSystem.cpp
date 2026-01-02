#include <random>

#include "voxelSystem.hpp"
#include "voxelMesh.hpp"

void VoxelSystem::Init(std::shared_ptr<Coordinator> coordinator)
{
  gCoordinator = coordinator;
}

void VoxelSystem::Update(float deltaTime, const glm::vec3 &playerPos)
{
  const glm::ivec3 playerChunk = WorldToChunk(playerPos);
  const int radius = std::max(world.simulationRadius, world.renderRadius);

  // lod 0
  for (int x = -radius; x <= radius; ++x)
    for (int y = -2; y <= 2; ++y)
      for (int z = -radius; z <= radius; ++z)
      {
        glm::ivec3 coord = playerChunk + glm::ivec3(x, y, z);

        if (!ChunkExists(coord))
          CreateChunk(coord, 0);
      }
  /*
// lod 1
for (int x = -radius * 2; x <= radius * 2; ++x)
  for (int y = -2; y <= 2; ++y)
    for (int z = -radius * 2; z <= radius * 2; ++z)
    {
      glm::ivec3 coord = playerChunk + glm::ivec3(x, y, z);

      if (!ChunkExists(coord))
        CreateChunk(coord, 1);
    }

// lod 2
for (int x = -radius * 4; x <= radius * 4; ++x)
  for (int y = -2; y <= 2; ++y)
    for (int z = -radius * 4; z <= radius * 4; ++z)
    {
      glm::ivec3 coord = playerChunk + glm::ivec3(x, y, z);

      if (!ChunkExists(coord))
        CreateChunk(coord, 2);
    }

// lod 3
for (int x = -radius * 6; x <= radius * 6; ++x)
  for (int y = -2; y <= 2; ++y)
    for (int z = -radius * 6; z <= radius * 6; ++z)
    {
      glm::ivec3 coord = playerChunk + glm::ivec3(x, y, z);

      if (!ChunkExists(coord))
        CreateChunk(coord, 3);
    }

// lod 4
for (int x = -radius * 8; x <= radius * 8; ++x)
  for (int y = -2; y <= 2; ++y)
    for (int z = -radius * 8; z <= radius * 8; ++z)
    {
      glm::ivec3 coord = playerChunk + glm::ivec3(x, y, z);

      if (!ChunkExists(coord))
        CreateChunk(coord, 4);
    }*/

  UnloadDistantChunks(playerChunk);
}

void VoxelSystem::UnloadDistantChunks(const glm::ivec3 &playerChunk)
{
  const int radius = std::max(world.simulationRadius, world.renderRadius);

  std::vector<glm::ivec3> chunksToRemove;
  chunksToRemove.reserve(world.chunkMap.size());

  for (const auto &[chunkPos, entity] : world.chunkMap)
  {
    glm::ivec3 delta = chunkPos - playerChunk;

    if (std::abs(delta.x) > radius * 8 + 1 ||
        std::abs(delta.y) > radius * 8 + 1 ||
        std::abs(delta.z) > radius * 8 + 1)
    {
      chunksToRemove.push_back(chunkPos);
    }
  }

  for (const glm::ivec3 &chunkPos : chunksToRemove)
  {
    Entity e = world.chunkMap.at(chunkPos);

    if (gCoordinator->HasComponent<MeshComponent>(e))
    {
      gCoordinator->GetComponent<MeshComponent>(e).mesh->Cleanup();
    }
    if (gCoordinator->HasComponent<VoxelMeshComponent>(e))
    {
      gCoordinator->GetComponent<VoxelMeshComponent>(e).mesh->Cleanup();
    }

    gCoordinator->DestroyEntity(e);
    world.chunkMap.erase(chunkPos);
  }
}

bool VoxelSystem::ChunkExists(const glm::ivec3 &coord)
{
  auto it = world.chunkMap.find(coord);
  if (it == world.chunkMap.end())
  {
    return false;
  }
  return true;
}

void VoxelSystem::CreateChunk(const glm::ivec3 &coord, int lod)
{
  Entity chunk = gCoordinator->CreateEntity();

  ChunkComponent cc = ChunkComponent(world.chunkWidth, world.chunkLength, world.chunkHeight, nextGPUIndex);
  nextGPUIndex++;
  cc.worldPosition = coord;
  cc.chunkState = ChunkState::NeedsMeshing;
  cc.chunkLOD = lod;

  gCoordinator->AddComponent<ChunkComponent>(chunk, std::move(cc));

  world.chunkMap[coord] = chunk;

  GenerateVoxelData(chunk);
}

int VoxelSystem::getIndex(int x, int y, int z)
{
  int flippedY = world.chunkHeight - y - 1;
  return x + world.chunkWidth * z + world.chunkWidth * world.chunkLength * flippedY;
}

uint32_t hash2(int x, int y, uint32_t seed)
{
  uint32_t h = seed;
  h ^= x * 0x27d4eb2d;
  h ^= y * 0x165667b1;
  h *= 0xc2b2ae35;
  return h;
}

glm::vec2 gradient2(int x, int y, uint32_t seed)
{
  uint32_t h = hash2(x, y, seed);
  h &= 7; // 8 directions

  switch (h)
  {
  case 0:
    return {1, 0};
  case 1:
    return {-1, 0};
  case 2:
    return {0, 1};
  case 3:
    return {0, -1};
  case 4:
    return {1, 1};
  case 5:
    return {-1, 1};
  case 6:
    return {1, -1};
  default:
    return {-1, -1};
  }
}

float fade(float t)
{
  return t * t * t * (t * (t * 6 - 15) + 10);
}

float perlin2(float x, float y, uint32_t seed)
{
  int x0 = int(floor(x));
  int y0 = int(floor(y));
  int x1 = x0 + 1;
  int y1 = y0 + 1;

  float sx = x - float(x0);
  float sy = y - float(y0);

  glm::vec2 g00 = gradient2(x0, y0, seed);
  glm::vec2 g10 = gradient2(x1, y0, seed);
  glm::vec2 g01 = gradient2(x0, y1, seed);
  glm::vec2 g11 = gradient2(x1, y1, seed);

  glm::vec2 d00 = {sx, sy};
  glm::vec2 d10 = {sx - 1, sy};
  glm::vec2 d01 = {sx, sy - 1};
  glm::vec2 d11 = {sx - 1, sy - 1};

  float n00 = glm::dot(g00, d00);
  float n10 = glm::dot(g10, d10);
  float n01 = glm::dot(g01, d01);
  float n11 = glm::dot(g11, d11);

  float u = fade(sx);
  float v = fade(sy);

  float nx0 = std::lerp(n00, n10, u);
  float nx1 = std::lerp(n01, n11, u);

  return std::lerp(nx0, nx1, v);
}

// value must be from -1 to 1
float normalize(float value)
{
  return value * 0.5f + 0.5f;
}

float perlin2Layered(float x, float y, uint32_t seed, int octaves, float frequency, float lacunarity, float gain)
{
  float value = 0.0f;
  float amp = 1.0f;
  float freq = frequency;
  float maxAmp = 0.0f;

  for (int i = 0; i < octaves; i++)
  {
    value += perlin2(x * freq, y * freq, seed + i * 1013) * amp;
    maxAmp += amp;

    amp *= gain;
    freq *= lacunarity;
  }

  return value / maxAmp;
}

float random2(int x, int y, uint32_t seed)
{
  return (hash2(x, y, seed) & 0xFFFFFF) / float(0xFFFFFF);
}

float randomRangeBiased2(int x, int y, uint32_t seed, float min, float max, float bias = 0)
{
  float r = random2(x, y, seed);

  if (bias != 0.0f)
  {
    // bias > 0  -> bias toward max
    // bias < 0  -> bias toward min
    float k = 1.0f + std::abs(bias);
    if (bias > 0.0f)
      r = std::pow(r, 1.0f / k);
    else
      r = 1.0f - std::pow(1.0f - r, 1.0f / k);
  }

  return min + r * (max - min);
}

uint32_t hash3(int x, int y, int z, uint32_t seed)
{
  uint32_t h = seed;
  h ^= x * 0x27d4eb2d;
  h ^= y * 0x165667b1;
  h ^= z * 0x85ebca6b;
  h *= 0xc2b2ae35;
  return h;
}

float random3(int x, int y, int z, uint32_t seed)
{
  return (hash3(x, y, z, seed) & 0xFFFFFF) / float(0xFFFFFF);
}

float randomRangeBiased3(int x, int y, int z, uint32_t seed, float min, float max, float bias = 0)
{
  float r = random3(x, y, z, seed);

  if (bias != 0.0f)
  {
    // bias > 0  -> bias toward max
    // bias < 0  -> bias toward min
    float k = 1.0f + std::abs(bias);
    if (bias > 0.0f)
      r = std::pow(r, 1.0f / k);
    else
      r = 1.0f - std::pow(1.0f - r, 1.0f / k);
  }

  return min + r * (max - min);
}

void VoxelSystem::GenerateVoxelData(Entity chunk)
{
  auto &chunkComp = gCoordinator->GetComponent<ChunkComponent>(chunk);
  chunkComp.voxelData.resize(world.chunkWidth * world.chunkHeight * world.chunkLength);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> heightDist(12, 16);

  const uint32_t stoneId = world.registry.nameToId.at("Stone");
  const uint32_t dirtId = world.registry.nameToId.at("Dirt");
  const uint32_t grassId = world.registry.nameToId.at("Grass");
  const uint32_t sandId = world.registry.nameToId.at("Sand");
  const uint32_t snowId = world.registry.nameToId.at("Snow");
  const uint32_t airId = world.registry.nameToId.at("Air");

  int worldBaseX = chunkComp.worldPosition.x * world.chunkWidth;
  int worldBaseZ = chunkComp.worldPosition.z * world.chunkLength;
  int worldBaseY = chunkComp.worldPosition.y * world.chunkHeight;
  for (int x = 0; x < world.chunkWidth; x++)
  {
    for (int z = 0; z < world.chunkLength; z++)
    {
      int worldX = x + worldBaseX;
      int worldZ = z + worldBaseZ;

      // n is between -1 and 1
      float n = perlin2Layered(worldX, worldZ, world.seed, 4, 0.03f, 2.0f, 0.7f);

      // n is between 0 and 1
      n = normalize(n);

      // terrainHeight is between 12 and 64
      int terrainHeight = (n * (64 - 12)) + 12;

      for (int y = 0; y < world.chunkHeight; y++)
      {
        int worldY = worldBaseY + y;
        int index = getIndex(x, y, z);

        if (worldY > terrainHeight)
        {
          chunkComp.voxelData[index] = {airId};
          continue;
        }

        int depth = terrainHeight - worldY;

        if (depth == 0)
        {
          // Surface block
          if (terrainHeight > 60)
            chunkComp.voxelData[index] = {snowId};
          else if (terrainHeight < 30)
            chunkComp.voxelData[index] = {sandId};
          else
            chunkComp.voxelData[index] = {grassId};
        }
        else if (depth <= 3)
        {
          chunkComp.voxelData[index] = {dirtId};
        }
        else
        {
          chunkComp.voxelData[index] = {stoneId};
        }
      }
    }
  }
}

glm::ivec3 VoxelSystem::WorldToChunk(const glm::vec3 &pos) const
{
  return {
      floor(pos.x / world.chunkWidth),
      floor(pos.y / world.chunkHeight),
      floor(pos.z / world.chunkLength)};
}

glm::ivec3 VoxelSystem::WorldToLocal(const glm::ivec3 &worldPos) const
{
  return {
      worldPos.x % world.chunkWidth,
      worldPos.y % world.chunkHeight,
      worldPos.z % world.chunkLength};
}

Voxel &AirVoxel()
{
  static Voxel air{0};
  return air;
}

Voxel &VoxelSystem::GetVoxel(const glm::ivec3 &worldPos)
{
  glm::ivec3 chunkCoord = WorldToChunk(worldPos);
  glm::ivec3 local = WorldToLocal(worldPos);

  auto it = world.chunkMap.find(chunkCoord);
  if (it == world.chunkMap.end())
    return AirVoxel();

  ChunkComponent &chunk = gCoordinator->GetComponent<ChunkComponent>(it->second);

  int index = local.x + world.chunkWidth * (local.z + world.chunkLength * local.y);

  return chunk.voxelData[index];
}

void VoxelSystem::SetVoxel(const glm::ivec3 &pos, uint32_t blockId)
{
  Voxel &v = GetVoxel(pos);
  v.type = blockId;

  MarkChunkDirty(WorldToChunk(pos));

  if (IsBorderVoxel(pos))
    MarkNeighborChunksDirty(pos);
}

void VoxelSystem::MarkChunkDirty(const glm::ivec3 &chunkPos)
{
  if (!ChunkExists(chunkPos))
    return;

  ChunkComponent &chunk = gCoordinator->GetComponent<ChunkComponent>(world.chunkMap.at(chunkPos));
  chunk.chunkState = ChunkState::NeedsMeshing;
}

bool VoxelSystem::IsBorderVoxel(const glm::ivec3 &worldPos) const
{
  glm::ivec3 local = WorldToLocal(worldPos);

  if (local.x < 0)
    local.x += world.chunkWidth;
  if (local.y < 0)
    local.y += world.chunkHeight;
  if (local.z < 0)
    local.z += world.chunkLength;

  return local.x == 0 ||
         local.y == 0 ||
         local.z == 0 ||
         local.x == world.chunkWidth - 1 ||
         local.y == world.chunkHeight - 1 ||
         local.z == world.chunkLength - 1;
}

void VoxelSystem::MarkNeighborChunksDirty(const glm::ivec3 &worldPos)
{
  glm::ivec3 chunkPos = WorldToChunk(worldPos);

  glm::ivec3 local = WorldToLocal(worldPos);

  if (local.x < 0)
    local.x += world.chunkWidth;
  if (local.y < 0)
    local.y += world.chunkHeight;
  if (local.z < 0)
    local.z += world.chunkLength;

  if (local.x == 0)
    MarkChunkDirty(chunkPos + glm::ivec3(-1, 0, 0));
  else if (local.x == world.chunkWidth - 1)
    MarkChunkDirty(chunkPos + glm::ivec3(1, 0, 0));

  if (local.y == 0)
    MarkChunkDirty(chunkPos + glm::ivec3(0, -1, 0));
  else if (local.y == world.chunkHeight - 1)
    MarkChunkDirty(chunkPos + glm::ivec3(0, 1, 0));

  if (local.z == 0)
    MarkChunkDirty(chunkPos + glm::ivec3(0, 0, -1));
  else if (local.z == world.chunkLength - 1)
    MarkChunkDirty(chunkPos + glm::ivec3(0, 0, 1));
}