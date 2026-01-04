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
    std::vector<Voxel> voxelData; // might want to turn it into a custom allocated type because the size stays constant after initialization

    ChunkState chunkState = ChunkState::Clean;
    int chunkLOD = 0;

    glm::ivec3 worldPosition;

    uint32_t gpuIndex = -1; // used for indexing into storage buffer for model matrix

    ChunkComponent()
    {
        chunkWidth = 0;
        chunkLength = 0;
        chunkHeight = 0;
    }

    ChunkComponent(int w, int l, int h, uint32_t uniqueIndex)
    {
        chunkWidth = w;
        chunkLength = l;
        chunkHeight = h;
        gpuIndex = uniqueIndex;
    }

    int getWidth()
    {
        return chunkWidth;
    }

    int getLength()
    {
        return chunkLength;
    }

    int getHeight()
    {
        return chunkHeight;
    }

private:
    int chunkWidth;
    int chunkLength;
    int chunkHeight;
};

struct WorldComponent
{
    BlockRegistry registry;
    int chunkWidth;
    int chunkLength;
    int chunkHeight;

    glm::ivec3 simulationRadius; // in chunks

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