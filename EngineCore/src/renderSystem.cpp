#include "renderSystem.hpp"
#include "renderer.hpp"
#include "mesh.hpp"
#include "uniformData.hpp"

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
    if (!gCoordinator->HasComponent<MeshComponent>(entity))
    {
      continue;
    }

    auto &transform = gCoordinator->GetComponent<TransformComponent>(entity);

    UniformBufferObject ubo;
    ubo.model = transform.GetMatrix();
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
}