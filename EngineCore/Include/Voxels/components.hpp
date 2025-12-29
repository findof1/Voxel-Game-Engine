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

struct ChunkComponent // turns an entity into a voxel chunk
{
    std::vector<Voxel> voxelData; // might want to turn it into a custom allocated type because the size stays constant after initialization

    ChunkState chunkState = ChunkState::Clean;

    glm::ivec3 worldPosition;

    ChunkComponent()
    {
    }
};

struct WorldComponent
{
    BlockRegistry registry;
    int chunkWidth;
    int chunkLength;
    int chunkHeight;

    int renderRadius; // in chunks
    int simulationRadius;

    std::unordered_map<glm::ivec3, Entity, IVec3Hash> chunkMap;

    uint64_t seed;
};