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

  // lod 0
  for (int x = -world.renderRadius0.x; x <= world.renderRadius0.x; ++x)
    for (int y = -world.renderRadius0.y; y <= world.renderRadius0.y; ++y)
      for (int z = -world.renderRadius0.z; z <= world.renderRadius0.z; ++z)
      {
        glm::ivec3 coord = playerChunk + glm::ivec3(x, y, z);

        if (!ChunkExists(coord))
          CreateChunk(coord, 0);
      }

  // lod 1
  for (int x = -world.renderRadius1.x; x <= world.renderRadius1.x; ++x)
    for (int y = -world.renderRadius1.y; y <= world.renderRadius1.y; ++y)
      for (int z = -world.renderRadius1.z; z <= world.renderRadius1.z; ++z)
      {
        glm::ivec3 coord = playerChunk + glm::ivec3(x, y, z);

        if (!ChunkExists(coord))
          CreateChunk(coord, 1);
      }

  // lod 2
  for (int x = -world.renderRadius2.x; x <= world.renderRadius2.x; ++x)
    for (int y = -world.renderRadius2.y; y <= world.renderRadius2.y; ++y)
      for (int z = -world.renderRadius2.z; z <= world.renderRadius2.z; ++z)
      {
        glm::ivec3 coord = playerChunk + glm::ivec3(x, y, z);

        if (!ChunkExists(coord))
          CreateChunk(coord, 2);
      }

  // lod 3
  for (int x = -world.renderRadius3.x; x <= world.renderRadius3.x; ++x)
    for (int y = -world.renderRadius3.y; y <= world.renderRadius3.y; ++y)
      for (int z = -world.renderRadius3.z; z <= world.renderRadius3.z; ++z)
      {
        glm::ivec3 coord = playerChunk + glm::ivec3(x, y, z);

        if (!ChunkExists(coord))
          CreateChunk(coord, 3);
      }

  // lod 4
  for (int x = -world.renderRadius4.x; x <= world.renderRadius4.x; ++x)
    for (int y = -world.renderRadius4.y; y <= world.renderRadius4.y; ++y)
      for (int z = -world.renderRadius4.z; z <= world.renderRadius4.z; ++z)
      {
        glm::ivec3 coord = playerChunk + glm::ivec3(x, y, z);

        if (!ChunkExists(coord))
          CreateChunk(coord, 4);
      }

  UnloadDistantChunks(playerChunk);
}

void VoxelSystem::UnloadDistantChunks(const glm::ivec3 &playerChunk)
{

  std::vector<glm::ivec3> chunksToRemove;
  chunksToRemove.reserve(world.chunkMap.size());

  for (const auto &[chunkPos, entity] : world.chunkMap)
  {
    glm::ivec3 delta = chunkPos - playerChunk;
    constexpr float padding = 1.25;
    if (std::abs(delta.x) > world.renderRadius4.x * padding ||
        std::abs(delta.y) > world.renderRadius4.y * padding ||
        std::abs(delta.z) > world.renderRadius4.z * padding)
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

  ChunkComponent cc = ChunkComponent(nextGPUIndex);
  nextGPUIndex++;
  cc.worldPosition = coord;
  cc.chunkState = ChunkState::NeedsMeshing;
  cc.chunkLOD = lod;

  gCoordinator->AddComponent<ChunkComponent>(chunk, std::move(cc));

  world.chunkMap[coord] = chunk;

  GenerateVoxelData(chunk);
}

ChunkComponent &VoxelSystem::StartGeneratingVoxelData(Entity chunk)
{
  auto &chunkComp = gCoordinator->GetComponent<ChunkComponent>(chunk);
  chunkComp.voxelData.resize(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
  return chunkComp;
}

int VoxelSystem::getIndex(int x, int y, int z)
{
  int flippedY = CHUNK_SIZE - y - 1;
  return x + CHUNK_SIZE * z + CHUNK_SIZE * CHUNK_SIZE * flippedY;
}

glm::ivec3 VoxelSystem::WorldToChunk(const glm::vec3 &pos) const
{
  return {
      floor(pos.x / CHUNK_SIZE),
      floor(pos.y / CHUNK_SIZE),
      floor(pos.z / CHUNK_SIZE)};
}

glm::ivec3 VoxelSystem::WorldToLocal(const glm::ivec3 &worldPos) const
{
  return {
      worldPos.x % CHUNK_SIZE,
      worldPos.y % CHUNK_SIZE,
      worldPos.z % CHUNK_SIZE};
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

  int index = local.x + CHUNK_SIZE * (local.z + CHUNK_SIZE * local.y);

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
    local.x += CHUNK_SIZE;
  if (local.y < 0)
    local.y += CHUNK_SIZE;
  if (local.z < 0)
    local.z += CHUNK_SIZE;

  return local.x == 0 ||
         local.y == 0 ||
         local.z == 0 ||
         local.x == CHUNK_SIZE - 1 ||
         local.y == CHUNK_SIZE - 1 ||
         local.z == CHUNK_SIZE - 1;
}

void VoxelSystem::MarkNeighborChunksDirty(const glm::ivec3 &worldPos)
{
  glm::ivec3 chunkPos = WorldToChunk(worldPos);

  glm::ivec3 local = WorldToLocal(worldPos);

  if (local.x < 0)
    local.x += CHUNK_SIZE;
  if (local.y < 0)
    local.y += CHUNK_SIZE;
  if (local.z < 0)
    local.z += CHUNK_SIZE;

  if (local.x == 0)
    MarkChunkDirty(chunkPos + glm::ivec3(-1, 0, 0));
  else if (local.x == CHUNK_SIZE - 1)
    MarkChunkDirty(chunkPos + glm::ivec3(1, 0, 0));

  if (local.y == 0)
    MarkChunkDirty(chunkPos + glm::ivec3(0, -1, 0));
  else if (local.y == CHUNK_SIZE - 1)
    MarkChunkDirty(chunkPos + glm::ivec3(0, 1, 0));

  if (local.z == 0)
    MarkChunkDirty(chunkPos + glm::ivec3(0, 0, -1));
  else if (local.z == CHUNK_SIZE - 1)
    MarkChunkDirty(chunkPos + glm::ivec3(0, 0, 1));
}