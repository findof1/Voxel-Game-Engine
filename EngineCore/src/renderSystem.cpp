#include "renderSystem.hpp"
#include "renderer.hpp"
#include "mesh.hpp"
#include "uniformData.hpp"
#include "Voxels/components.hpp"
#include "voxelMesh.hpp"

void RenderSystem::Init(std::shared_ptr<Coordinator> coordinator, int screenWidth, int screenHeight)
{
  gCoordinator = coordinator;
  this->screenWidth = screenWidth;
  this->screenHeight = screenHeight;
}

void RenderSystem::Update(Renderer &renderer, float deltaTime, const Camera &camera)
{
  renderer.startFrame();
  RenderScene(renderer, deltaTime, camera);
  renderer.endFrame();
}

void RenderSystem::RenderScene(Renderer &renderer, float deltaTime, const Camera &camera)
{
  for (auto const &entity : mEntities)
  {
    if (gCoordinator->HasComponent<MeshComponent>(entity))
    {

      UniformBufferObject ubo;
      ubo.model = getWorldMatrix(entity);
      ubo.view = camera.getViewMatrix();
      ubo.proj = camera.getProjectionMatrix(renderer.swapChainObjects.swapChainExtent.width / (float)renderer.swapChainObjects.swapChainExtent.height);

      auto &mesh = gCoordinator->GetComponent<MeshComponent>(entity);
      mesh.mesh->UpdateUBO(ubo);

      VkCommandBuffer commandBuffer = renderer.commandBuffers[renderer.currentFrame];
      VkExtent2D extent = renderer.swapChainObjects.swapChainExtent;
      bindGraphicsPipeline(commandBuffer, renderer.pipeline);

      VkViewport viewport = makeViewport(extent);
      setViewport(commandBuffer, viewport);

      VkRect2D scissor = makeScissor(extent);
      setScissor(commandBuffer, scissor);

      mesh.mesh->Draw();
    }

    if (gCoordinator->HasComponent<VoxelMeshComponent>(entity))
    {
      UniformBufferObject ubo;
      ubo.model = getWorldMatrix(entity);
      ubo.view = camera.getViewMatrix();
      ubo.proj = camera.getProjectionMatrix(renderer.swapChainObjects.swapChainExtent.width / (float)renderer.swapChainObjects.swapChainExtent.height);

      auto &mesh = gCoordinator->GetComponent<VoxelMeshComponent>(entity);
      mesh.mesh->UpdateUBO(ubo);

      VkCommandBuffer commandBuffer = renderer.commandBuffers[renderer.currentFrame];
      VkExtent2D extent = renderer.swapChainObjects.swapChainExtent;
      bindGraphicsPipeline(commandBuffer, renderer.voxelPipeline);

      VkViewport viewport = makeViewport(extent);
      setViewport(commandBuffer, viewport);

      VkRect2D scissor = makeScissor(extent);
      setScissor(commandBuffer, scissor);

      mesh.mesh->Draw();
    }
  }
}

glm::mat4 RenderSystem::getWorldMatrix(Entity entity)
{
  glm::mat4 result(1.0f);

  Entity current = entity;

  while (current != -1 && gCoordinator->HasComponent<TransformComponent>(current))
  {
    auto &t = gCoordinator->GetComponent<TransformComponent>(current);
    result = t.GetMatrix() * result;

    if (!gCoordinator->HasComponent<Parent>(current))
      break;

    current = gCoordinator->GetComponent<Parent>(current).value;
  }

  return result;
}