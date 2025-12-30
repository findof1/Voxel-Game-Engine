#include "application.hpp"
#include <chrono>
#include <iostream>
#include "voxelMesh.hpp"

const std::vector<Vertex> vertices = {
    // Front (+Z)
    {{-0.5f, -0.5f, 0.5f}, {1, 0, 0}, {0, 0}},
    {{0.5f, -0.5f, 0.5f}, {1, 0, 0}, {1, 0}},
    {{0.5f, 0.5f, 0.5f}, {1, 0, 0}, {1, 1}},
    {{-0.5f, 0.5f, 0.5f}, {1, 0, 0}, {0, 1}},

    // Back (-Z)
    {{0.5f, -0.5f, -0.5f}, {0, 1, 0}, {0, 0}},
    {{-0.5f, -0.5f, -0.5f}, {0, 1, 0}, {1, 0}},
    {{-0.5f, 0.5f, -0.5f}, {0, 1, 0}, {1, 1}},
    {{0.5f, 0.5f, -0.5f}, {0, 1, 0}, {0, 1}},

    // Left (-X)
    {{-0.5f, -0.5f, -0.5f}, {0, 0, 1}, {0, 0}},
    {{-0.5f, -0.5f, 0.5f}, {0, 0, 1}, {1, 0}},
    {{-0.5f, 0.5f, 0.5f}, {0, 0, 1}, {1, 1}},
    {{-0.5f, 0.5f, -0.5f}, {0, 0, 1}, {0, 1}},

    // Right (+X)
    {{0.5f, -0.5f, 0.5f}, {1, 1, 0}, {0, 0}},
    {{0.5f, -0.5f, -0.5f}, {1, 1, 0}, {1, 0}},
    {{0.5f, 0.5f, -0.5f}, {1, 1, 0}, {1, 1}},
    {{0.5f, 0.5f, 0.5f}, {1, 1, 0}, {0, 1}},

    // Top (+Y)
    {{-0.5f, 0.5f, 0.5f}, {0, 1, 1}, {0, 0}},
    {{0.5f, 0.5f, 0.5f}, {0, 1, 1}, {1, 0}},
    {{0.5f, 0.5f, -0.5f}, {0, 1, 1}, {1, 1}},
    {{-0.5f, 0.5f, -0.5f}, {0, 1, 1}, {0, 1}},

    // Bottom (-Y)
    {{-0.5f, -0.5f, -0.5f}, {1, 0, 1}, {0, 0}},
    {{0.5f, -0.5f, -0.5f}, {1, 0, 1}, {1, 0}},
    {{0.5f, -0.5f, 0.5f}, {1, 0, 1}, {1, 1}},
    {{-0.5f, -0.5f, 0.5f}, {1, 0, 1}, {0, 1}}};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20};

void processInput(GLFWwindow *window, float deltaTime, Camera &camera)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.processKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.processKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.processKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.processKeyboard(RIGHT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    camera.processKeyboard(UP, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    camera.processKeyboard(DOWN, deltaTime);
}

Application::Application() : window(initWindow(windowWidth, windowHeight, "Voxel Game Engine", this)), renderer(window)
{
  if (!window)
  {
    std::cerr << "Failed to initialize window" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void Application::run()
{
  renderer.init();

  coordinator = std::make_shared<Coordinator>();
  coordinator->Init();

  // Register components
  coordinator->RegisterComponent<TransformComponent>();
  coordinator->RegisterComponent<MeshComponent>();
  coordinator->RegisterComponent<VoxelMeshComponent>();
  coordinator->RegisterComponent<Parent>();
  coordinator->RegisterComponent<Children>();
  coordinator->RegisterComponent<ChunkComponent>();
  coordinator->RegisterComponent<WorldComponent>();

  // Register and configure render system
  renderSystem = coordinator->RegisterSystem<RenderSystem>();
  {
    Signature signature;
    signature.set(coordinator->GetComponentType<TransformComponent>());
    coordinator->SetSystemSignature<RenderSystem>(signature);
  }
  renderSystem->Init(coordinator, windowWidth, windowHeight);

  // Create a world entity
  Entity world = coordinator->CreateEntity();
  {
    WorldComponent worldComponent{};
    worldComponent.chunkHeight = 32;
    worldComponent.chunkWidth = 32;
    worldComponent.chunkLength = 32;
    worldComponent.renderRadius = 3;
    worldComponent.simulationRadius = 3;
    worldComponent.seed = 21;
    coordinator->AddComponent(world, worldComponent);
  }
  WorldComponent &worldComp = coordinator->GetComponent<WorldComponent>(world);

  voxelSystem = coordinator->RegisterSystem<VoxelSystem>(worldComp);
  {
    Signature signature;
    signature.set(coordinator->GetComponentType<WorldComponent>());
    signature.set(coordinator->GetComponentType<ChunkComponent>());
    coordinator->SetSystemSignature<VoxelSystem>(signature);
  }
  voxelSystem->Init(coordinator);

  meshingSystem = coordinator->RegisterSystem<MeshingSystem>(worldComp);
  {
    Signature signature;
    signature.set(coordinator->GetComponentType<ChunkComponent>());
    coordinator->SetSystemSignature<MeshingSystem>(signature);
  }
  meshingSystem->Init(coordinator);

  auto addBlock = [&](const std::string &name, int top, int bottom, int side, int visible = true)
  {
    uint32_t id = worldComp.registry.blocks.size();
    BlockType block;
    block.name = name;
    block.visible = visible;
    block.textureTop = top;
    block.textureBottom = bottom;
    block.textureSide = side;

    worldComp.registry.blocks.push_back(block);
    worldComp.registry.nameToId[name] = id;
  };

  std::vector<std::string> filePaths;
  filePaths.resize(10);
  filePaths[0] = "Assets/textures/Tiles/dirt.png";
  filePaths[1] = "Assets/textures/Tiles/dirt_grass.png";
  filePaths[2] = "Assets/textures/Tiles/grass_top.png";
  filePaths[3] = "Assets/textures/Tiles/stone.png";
  filePaths[4] = "Assets/textures/Tiles/sand.png";
  filePaths[5] = "Assets/textures/Tiles/trunk_side.png";
  filePaths[6] = "Assets/textures/Tiles/trunk_top.png";
  filePaths[7] = "Assets/textures/Tiles/leaves.png";
  filePaths[8] = "Assets/textures/Tiles/redsand.png";
  filePaths[9] = "Assets/textures/Tiles/snow.png";

  addBlock("Air", -1, -1, -1, false);
  addBlock("Grass", 2, 0, 1);
  addBlock("Dirt", 0, 0, 0);
  addBlock("Stone", 3, 3, 3);
  addBlock("Sand", 4, 4, 4);
  addBlock("Oak Log", 6, 6, 5);
  addBlock("Oak Leaves", 7, 7, 7);
  addBlock("Red Sand", 8, 8, 8);
  addBlock("Snow", 9, 9, 9);

  Texture wood = renderer.createTexutre("Wood", "Assets/textures/wood.png");

  renderer.createTexutreArray("Voxel Textures", filePaths);

  // Create a cube entity
  {
    Entity cube = coordinator->CreateEntity();
    TransformComponent cubeTransform{};
    cubeTransform.translation = {0.0f, 0.0f, -5.0f};
    cubeTransform.scale = {1.0f, 1.0f, 1.0f};
    coordinator->AddComponent(cube, cubeTransform);
    auto mesh = std::make_shared<Mesh>(renderer);
    mesh->Init(wood, vertices, indices);
    coordinator->AddComponent(cube, MeshComponent{mesh});
  }

  // Create a vase entity by loading a obj
  {
    Entity vase = coordinator->CreateEntity();
    TransformComponent vaseTransform{};
    vaseTransform.translation = {0.0f, 0.0f, 15.0f};
    vaseTransform.scale = {1.0f, 1.0f, 1.0f};
    coordinator->AddComponent(vase, vaseTransform);
    LoadModel(vase, coordinator, renderer, wood, "Assets/models/smooth_vase.obj");
  }

  mainLoop();

  cleanup();
}

void Application::mainLoop()
{
  float lastTime = glfwGetTime();
  float fpsTimer = 0.0f;
  int frameCount = 0;

  while (!glfwWindowShouldClose(window))
  {
    float currentTime = glfwGetTime();
    float dt = currentTime - lastTime;
    lastTime = currentTime;

    fpsTimer += dt;
    frameCount++;

    if (fpsTimer >= 1.0f) // every second
    {
      float fps = frameCount / fpsTimer;
      printf("FPS: %.2f\n", fps);

      fpsTimer = 0.0f;
      frameCount = 0;
    }

    glfwPollEvents();
    processInput(window, dt, camera);
    voxelSystem->Update(dt, glm::vec3(camera.Position.x, -camera.Position.y, camera.Position.z));
    meshingSystem->Update(renderer.getTexture("Voxel Textures"), renderer);
    renderSystem->Update(renderer, dt, camera);
  }
}

void Application::cleanup()
{
  vkDeviceWaitIdle(renderer.device);

  for (auto &entity : renderSystem->mEntities)
  {
    if (coordinator->HasComponent<MeshComponent>(entity))
    {
      auto &meshComponent = coordinator->GetComponent<MeshComponent>(entity);
      meshComponent.mesh->Cleanup();
    }

    if (coordinator->HasComponent<VoxelMeshComponent>(entity))
    {
      auto &meshComponent = coordinator->GetComponent<VoxelMeshComponent>(entity);
      meshComponent.mesh->Cleanup();
    }
  }

  renderSystem.reset();
  coordinator.reset();

  renderer.cleanup();

  closeWindow(window);
}