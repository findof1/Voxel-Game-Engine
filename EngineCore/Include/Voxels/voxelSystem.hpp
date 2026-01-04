#pragma once
#include <memory>
#include <utility>
#include <functional>

#include "coordinator.hpp"
#include "types.hpp"
#include "Voxels/components.hpp"
#include "camera.hpp"
#include "ECS/components.hpp"
#include "mesh.hpp"

#include "FastNoiseLite.h"

constexpr uint32_t MAX_CHUNKS = 32768;

class VoxelSystem : public System
{
public:
    std::shared_ptr<Coordinator> gCoordinator;

    VoxelSystem(WorldComponent &world) : world(world)
    {
    }
    void Init(std::shared_ptr<Coordinator> coordinator);
    void Update(float deltaTime, const glm::vec3 &playerPos);

    WorldComponent &world;

    void UnloadDistantChunks(const glm::ivec3 &playerChunk);
    bool ChunkExists(const glm::ivec3 &coord);
    void CreateChunk(const glm::ivec3 &coord, int lod);

    ChunkComponent &StartGeneratingVoxelData(Entity chunk);
    virtual void GenerateVoxelData(Entity chunk) = 0; // World Generation Logic

    glm::ivec3 WorldToChunk(const glm::vec3 &pos) const;
    glm::ivec3 WorldToLocal(const glm::ivec3 &worldPos) const;
    int getIndex(int x, int y, int z);
    Voxel &GetVoxel(const glm::ivec3 &worldPos);
    void SetVoxel(const glm::ivec3 &pos, uint32_t blockId);
    void MarkChunkDirty(const glm::ivec3 &chunkPos);
    bool IsBorderVoxel(const glm::ivec3 &worldPos) const;
    void MarkNeighborChunksDirty(const glm::ivec3 &worldPos);
};