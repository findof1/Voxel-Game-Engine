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

bool FrustumIntersects(const Frustum &frustum, const glm::vec3 &center, const glm::vec3 &halfSize)
{
  for (int i = 0; i < 6; i++)
  {
    const glm::vec4 &p = frustum.planes[i];

    glm::vec3 positive = center;
    positive.x += (p.x >= 0.0f) ? halfSize.x : -halfSize.x;
    positive.y += (p.y >= 0.0f) ? halfSize.y : -halfSize.y;
    positive.z += (p.z >= 0.0f) ? halfSize.z : -halfSize.z;

    // Plane equation: ax + by + cz + d
    if (glm::dot(glm::vec3(p), positive) + p.w < 0)
      return false; // Outside this plane
  }

  return true; // Intersects all planes
}

void RenderSystem::RenderScene(Renderer &renderer, float deltaTime, const Camera &camera)
{
  /*
  glm::mat4 view = camera.getViewMatrix();
  glm::mat4 proj = camera.getProjectionMatrix(renderer.swapChainObjects.swapChainExtent.width / (float)renderer.swapChainObjects.swapChainExtent.height);
  glm::mat4 viewProj = proj * view;
  Frustum frustum = camera.extractFrustumPlanes(viewProj);*/

  VkExtent2D extent = renderer.swapChainObjects.swapChainExtent;

  float aspect = extent.width / (float)extent.height;

  UniformBufferObject ubo{};
  ubo.view = camera.getViewMatrix();
  ubo.proj = camera.getProjectionMatrix(aspect);

  uint32_t currentFrame = renderer.currentFrame;
  VkCommandBuffer cmdBuff = renderer.commandBuffers[currentFrame];
  VkDescriptorSet cameraSet = renderer.cameraSets[currentFrame];

  memcpy(renderer.uniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));

  for (auto const &entity : mEntities)
  {
    if (gCoordinator->HasComponent<MeshComponent>(entity))
    {
      auto &mesh = gCoordinator->GetComponent<MeshComponent>(entity);

      bindGraphicsPipeline(cmdBuff, renderer.pipeline);

      VkViewport viewport = makeViewport(extent);
      setViewport(cmdBuff, viewport);

      VkRect2D scissor = makeScissor(extent);
      setScissor(cmdBuff, scissor);

      std::vector<VkDescriptorSet> sets = {cameraSet, mesh.mesh->texture.imageSet};
      bindDescriptorSets(sets, cmdBuff, renderer.pipelineLayout);

      PushConstants pc{};
      pc.model = getWorldMatrix(entity);

      vkCmdPushConstants(cmdBuff, renderer.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &pc);

      mesh.mesh->Draw();
    }

    if (gCoordinator->HasComponent<VoxelMeshComponent>(entity) && gCoordinator->HasComponent<ChunkComponent>(entity))
    {
      /*
      if (gCoordinator->HasComponent<ChunkComponent>(entity))
      {
        auto &chunk = gCoordinator->GetComponent<ChunkComponent>(entity);
        int chunkWidth = chunk.getWidth();
        int chunkLength = chunk.getLength();
        int chunkHeight = chunk.getHeight();
        glm::vec3 chunkCenter = glm::vec3(chunk.worldPosition.x * chunkWidth + chunkWidth / 2.0f, (-chunk.worldPosition.y * chunkHeight + chunkHeight / 2.0f), chunk.worldPosition.z * chunkLength + chunkLength / 2.0f);

        glm::vec3 chunkHalf = glm::vec3(chunkWidth / 2.0f, chunkHeight / 2.0f, chunkLength / 2.0f);

        if (!FrustumIntersects(frustum, chunkCenter, chunkHalf))
          continue; // Skip rendering this chunk
      }*/

      auto &mesh = gCoordinator->GetComponent<VoxelMeshComponent>(entity);
      auto &chunk = gCoordinator->GetComponent<ChunkComponent>(entity);

      bindGraphicsPipeline(cmdBuff, renderer.voxelPipeline);

      VkViewport viewport = makeViewport(extent);
      setViewport(cmdBuff, viewport);

      VkRect2D scissor = makeScissor(extent);
      setScissor(cmdBuff, scissor);

      std::vector<VkDescriptorSet> sets = {cameraSet, renderer.voxelSet, mesh.mesh->texture.imageSet};
      bindDescriptorSets(sets, cmdBuff, renderer.voxelPipelineLayout);

      VoxelPushConstants pc{};
      pc.objectId = chunk.gpuIndex;

      vkCmdPushConstants(cmdBuff, renderer.voxelPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VoxelPushConstants), &pc);

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