#pragma once
#include "renderer.hpp"
#include "camera.hpp"

#include "componentArray.hpp"
#include "components.hpp"
#include "componentManager.hpp"
#include "coordinator.hpp"
#include "entityManager.hpp"
#include "renderSystem.hpp"
#include "mesh.hpp"
#include "modelLoading.hpp"
#include "Voxels/components.hpp"
#include "voxelSystem.hpp"
#include "meshingSystem.hpp"

class Application
{
public:
  int windowWidth = 800;
  int windowHeight = 600;
  GLFWwindow *window;
  Renderer renderer;
  Camera camera;

  std::shared_ptr<Coordinator> coordinator;
  std::shared_ptr<VoxelSystem> voxelSystem;
  std::shared_ptr<MeshingSystem> meshingSystem;
  std::shared_ptr<RenderSystem> renderSystem;

  float lastX = 800.0f / 2.0f;
  float lastY = 600.0f / 2.0f;
  bool firstMouse = true;

  Application();
  void run();

private:
  void mainLoop();
  void cleanup();
};
