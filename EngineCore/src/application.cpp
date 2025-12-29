#include "application.hpp"
#include <chrono>
#include <iostream>

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
    worldComponent.chunkHeight = 16;
    worldComponent.chunkWidth = 16;
    worldComponent.chunkLength = 16;
    worldComponent.renderRadius = 1;
    worldComponent.simulationRadius = 1;
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

  addBlock("Air", -1, -1, -1, false);

  addBlock("Grass", 0, 2, 1);

  addBlock("Dirt", 2, 2, 2);

  addBlock("Stone", 3, 3, 3);

  addBlock("Sand", 4, 4, 4);

  Texture wood = renderer.createTexutre("Wood", "Assets/textures/wood.png");
  std::vector<std::string> filePaths = {"Assets/textures/wall.png", "Assets/textures/fire.png"};
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
    voxelSystem->Update(dt, glm::vec3(0));
    meshingSystem->Update(renderer.getTexture("Wood"), renderer);
    renderSystem->Update(renderer, dt, camera);
  }
}

void Application::cleanup()
{
  vkDeviceWaitIdle(renderer.device);

  for (auto &entity : renderSystem->mEntities)
  {
    if (!coordinator->HasComponent<MeshComponent>(entity))
    {
      continue;
    }
    auto &meshComponent = coordinator->GetComponent<MeshComponent>(entity);
    meshComponent.mesh->Cleanup();
  }

  renderSystem.reset();
  coordinator.reset();

  renderer.cleanup();

  closeWindow(window);
}