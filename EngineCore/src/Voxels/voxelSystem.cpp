#include <random>

#include "voxelSystem.hpp"

void VoxelSystem::Init(std::shared_ptr<Coordinator> coordinator)
{
  gCoordinator = coordinator;
}

void VoxelSystem::Update(float deltaTime, const glm::vec3 &playerPos)
{
  const glm::ivec3 playerChunk = WorldToChunk(playerPos);
  const int radius = std::max(world.simulationRadius, world.renderRadius);

  for (int x = -radius; x <= radius; ++x)
    for (int y = -radius; y <= radius; ++y)
      for (int z = -radius; z <= radius; ++z)
      {
        glm::ivec3 coord = playerChunk + glm::ivec3(x, y, z);

        if (!ChunkExists(coord))
          CreateChunk(coord);
      }

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

    if (std::abs(delta.x) > radius + 1 ||
        std::abs(delta.y) > radius + 1 ||
        std::abs(delta.z) > radius + 1)
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

void VoxelSystem::CreateChunk(const glm::ivec3 &coord)
{
  Entity chunk = gCoordinator->CreateEntity();

  ChunkComponent cc;
  cc.worldPosition = coord;
  cc.chunkState = ChunkState::NeedsMeshing;

  gCoordinator->AddComponent<ChunkComponent>(chunk, std::move(cc));

  world.chunkMap[coord] = chunk;

  GenerateVoxelData(chunk);
}

int VoxelSystem::getIndex(int x, int y, int z)
{
  return x + world.chunkWidth * (z + world.chunkLength * y);
}

void VoxelSystem::GenerateVoxelData(Entity chunk) // World Generation Logic
{
  auto &chunkComp = gCoordinator->GetComponent<ChunkComponent>(chunk);
  chunkComp.voxelData.resize(world.chunkWidth * world.chunkHeight * world.chunkLength);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> heightDist(12, 16);

  for (int x = 0; x < world.chunkWidth; x++)
  {
    for (int z = 0; z < world.chunkLength; z++)
    {
      int terrainHeight = heightDist(gen);

      for (int y = 0; y < world.chunkHeight; y++)
      {
        if (y + world.chunkHeight * chunkComp.worldPosition.y <= terrainHeight)
          chunkComp.voxelData[getIndex(x, world.chunkHeight - y - 1, z)] = {world.registry.nameToId.at("Stone")};
        else
          chunkComp.voxelData[getIndex(x, world.chunkHeight - y - 1, z)] = {world.registry.nameToId.at("Air")};
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