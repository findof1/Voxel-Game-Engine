#pragma once
#include <unordered_map>
#include <FastNoiseLite.h>
#include "Voxels/components.hpp"
#include "voxelSystem.hpp"

struct WorldFeatures
{
  float elevation;       // mountains vs oceans
  float erosion;         // smooth vs sharp
  float continentalness; // landmass vs sea
  float weirdness;       // noise warp / chaos
  float temperature;     // climate
  float humidity;        // moisture
};

struct Biome
{
  uint16_t airBlock;
  uint16_t topBlock;
  uint16_t fillerBlock;
  uint16_t stoneBlock;
  uint16_t waterBlock;
  uint16_t bottomBlock;

  uint8_t topDepth;
  uint8_t fillerDepth;

  WorldFeatures worldFeatures;
};

class DefaultVoxelSystem : public VoxelSystem
{
public:
  using VoxelSystem::VoxelSystem;
  FastNoiseLite elevation;
  FastNoiseLite erosion;
  FastNoiseLite continentalness;
  FastNoiseLite weirdness;
  FastNoiseLite temperature;
  FastNoiseLite humidity;
  std::unordered_map<std::string, Biome> biomes;

  DefaultVoxelSystem(WorldComponent &world) : VoxelSystem(world)
  {
    int seedOffsets = 1; // so seeds on different params are not correlated
    elevation.SetSeed(world.seed);
    elevation.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    elevation.SetFrequency(0.004f);
    elevation.SetFractalType(FastNoiseLite::FractalType_FBm);
    elevation.SetFractalOctaves(4);
    elevation.SetFractalLacunarity(4.0f);
    elevation.SetFractalGain(0.7f);

    erosion.SetSeed(world.seed + seedOffsets);
    erosion.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    erosion.SetFrequency(0.03f);

    continentalness.SetSeed(world.seed + 2 * seedOffsets);
    continentalness.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    continentalness.SetFrequency(0.004f);
    continentalness.SetFractalType(FastNoiseLite::FractalType_FBm);
    continentalness.SetFractalOctaves(4);
    continentalness.SetFractalLacunarity(1.5f);
    continentalness.SetFractalGain(0.7f);

    weirdness.SetSeed(world.seed + 3 * seedOffsets);
    weirdness.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    weirdness.SetFrequency(0.0014f);
    weirdness.SetFractalType(FastNoiseLite::FractalType_FBm);
    weirdness.SetFractalOctaves(8);
    weirdness.SetFractalLacunarity(1.5f);
    weirdness.SetFractalGain(0.9f);

    temperature.SetSeed(world.seed + 4 * seedOffsets);
    temperature.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    temperature.SetFrequency(0.01f);
    temperature.SetFractalType(FastNoiseLite::FractalType_FBm);
    temperature.SetFractalOctaves(2);
    temperature.SetFractalLacunarity(2.0f);
    temperature.SetFractalGain(0.3f);

    humidity.SetSeed(world.seed + 5 * seedOffsets);
    humidity.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    humidity.SetFrequency(0.017f);
    humidity.SetFractalType(FastNoiseLite::FractalType_FBm);
    humidity.SetFractalOctaves(2);
    humidity.SetFractalLacunarity(2.0f);
    humidity.SetFractalGain(0.3f);
  }

  void addBiome(Biome b, const std::string &name)
  {
    biomes.emplace(name, b);
  }

  float BiomeDistance(const Biome &b, const WorldFeatures &f)
  {
    float de = b.worldFeatures.elevation - f.elevation;
    float dr = b.worldFeatures.erosion - f.erosion;
    float dc = b.worldFeatures.continentalness - f.continentalness;
    float dw = b.worldFeatures.weirdness - f.weirdness;
    float dt = b.worldFeatures.temperature - f.temperature;
    float dh = b.worldFeatures.humidity - f.humidity;

    return de * de + dr * dr + dc * dc + dw * dw + dt * dt + dh * dh;
  }

  const Biome &chooseBiome(const WorldFeatures &features)
  {
    auto it = biomes.begin();
    if (it == biomes.end())
      throw std::runtime_error("No biomes registered");

    const Biome *best = &it->second;
    float bestDist = BiomeDistance(*best, features);

    for (auto &[_, biome] : biomes)
    {
      float dist = BiomeDistance(biome, features);
      if (dist < bestDist)
      {
        bestDist = dist;
        best = &biome;
      }
    }

    return *best;
  }

  WorldFeatures generateWorldFeatures(float x, float z)
  {
    WorldFeatures f;
    f.continentalness = 0.5 + continentalness.GetNoise(x, z) * 0.5;
    f.elevation = 0.5 + elevation.GetNoise(x, z) * 0.5;
    f.erosion = 0.5 + erosion.GetNoise(x, z) * 0.5;
    f.humidity = 0.5 + humidity.GetNoise(x, z) * 0.5;
    f.temperature = 0.5 + temperature.GetNoise(x, z) * 0.5;
    f.weirdness = 0.5 + weirdness.GetNoise(x, z) * 0.5;
    return f;
  }

  void createVoxel(ChunkComponent &chunk, float x, float y, float z, uint32_t blockType)
  {
    chunk.voxelData[getIndex(x, y, z)] = {blockType};
  }

  float computeTerrainHeight(const WorldFeatures &features, float minHeight, float maxHeight)
  {
    float landFactor = features.continentalness;

    float elevationFactor = features.elevation;
    elevationFactor = pow(elevationFactor, 1.5f);

    float erosionFactor = features.erosion * 0.5f;

    float weirdnessFactor = features.weirdness * 2.0f;

    float combined = landFactor * (elevationFactor + erosionFactor + weirdnessFactor);

    float height = minHeight + combined * (maxHeight - minHeight);

    return height;
  }

  void GenerateVoxelData(Entity chunk) override
  {
    auto &chunkComp = StartGeneratingVoxelData(chunk);

    int worldBaseX = chunkComp.worldPosition.x * CHUNK_SIZE;
    int worldBaseZ = chunkComp.worldPosition.z * CHUNK_SIZE;
    int worldBaseY = chunkComp.worldPosition.y * CHUNK_SIZE;

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
      int worldX = x + worldBaseX;
      for (int z = 0; z < CHUNK_SIZE; z++)
      {
        int worldZ = z + worldBaseZ;
        WorldFeatures features = generateWorldFeatures(worldX, worldZ);

        const Biome &biome = chooseBiome(features);

        int terrainHeight = computeTerrainHeight(features, world.minTerrainHeight, world.maxTerrainHeight);
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
          int worldY = worldBaseY + y;

          if (worldY > terrainHeight && worldY > world.waterLevel)
          {
            createVoxel(chunkComp, x, y, z, biome.airBlock);
            continue;
          }
          else if (worldY > terrainHeight && worldY < world.waterLevel)
          {
            createVoxel(chunkComp, x, y, z, biome.waterBlock);
            continue;
          }

          int depth = terrainHeight - worldY;

          if (depth < biome.topDepth)
          {
            createVoxel(chunkComp, x, y, z, biome.topBlock);
          }
          else if (depth <= biome.topDepth + biome.fillerDepth)
          {
            createVoxel(chunkComp, x, y, z, biome.fillerBlock);
          }
          else if (depth < terrainHeight)
          {
            createVoxel(chunkComp, x, y, z, biome.stoneBlock);
          }
          else
          {
            createVoxel(chunkComp, x, y, z, biome.bottomBlock);
          }
        }
      }
    }
  }
};