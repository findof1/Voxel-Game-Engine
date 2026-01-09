#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "voxel.hpp"
#include "entityManager.hpp"

class VoxelMesh;

#define CHUNK_SIZE 31

struct IVec3Hash
{
    std::size_t operator()(const glm::ivec3 &v) const noexcept
    {
        std::size_t h1 = std::hash<int>{}(v.x);
        std::size_t h2 = std::hash<int>{}(v.y);
        std::size_t h3 = std::hash<int>{}(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

enum class ChunkState
{
    Clean,
    NeedsMeshing,
    Meshing,
};

struct VoxelMeshComponent
{
    std::shared_ptr<VoxelMesh> mesh;

    VoxelMeshComponent() = default;
    VoxelMeshComponent(std::shared_ptr<VoxelMesh> m) : mesh(m) {}
};

static uint32_t nextGPUIndex = 0;

struct ChunkComponent // turns an entity into a voxel chunk
{
    std::vector<Voxel> voxelData;

    ChunkState chunkState = ChunkState::Clean;
    int chunkLOD = 0;

    glm::ivec3 worldPosition;

    uint32_t gpuIndex = -1; // used for indexing into storage buffer for model matrix

    ChunkComponent()
    {
    }

    ChunkComponent(uint32_t uniqueIndex)
    {
        gpuIndex = uniqueIndex;
    }
};

struct WorldComponent
{
    BlockRegistry registry;
    int waterLevel = 48;
    int minTerrainHeight = 32;
    int maxTerrainHeight = 64;

    // returns the id on success, -1 on failure
    static uint32_t getBlockID(WorldComponent &world, const std::string &name)
    {
        if (world.registry.nameToId.find(name) != world.registry.nameToId.end())
            return world.registry.nameToId.at(name);

        return -1;
    }

    // lod0
    glm::ivec3 renderRadius0; // in chunks

    // lod1
    glm::ivec3 renderRadius1;

    // lod2
    glm::ivec3 renderRadius2;

    // lod3
    glm::ivec3 renderRadius3;

    // lod3
    glm::ivec3 renderRadius4;

    std::unordered_map<glm::ivec3, Entity, IVec3Hash> chunkMap;

    uint64_t seed;
};