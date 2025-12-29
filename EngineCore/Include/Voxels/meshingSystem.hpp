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

class MeshingSystem : public System
{
public:
  std::shared_ptr<Coordinator> gCoordinator;

  MeshingSystem(WorldComponent &world) : world(world)
  {
  }

  void Init(std::shared_ptr<Coordinator> coordinator);
  void Update(Texture voxelTextures, Renderer &renderer);

  void CreateMesh(Texture voxelTextures, Renderer &renderer, Entity chunk);

private:
  WorldComponent &world;
};