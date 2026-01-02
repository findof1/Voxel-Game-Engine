#include "meshingSystem.hpp"
#include "renderer.hpp"

void EmitQuad(std::vector<VoxelVertex> &vertices, std::vector<uint32_t> &indices, glm::ivec3 pos, glm::ivec3 size, int axis, bool backFace, uint16_t texture, int step)
{
  pos *= step;
  size *= step;
  int u = (axis + 1) % 3;
  int v = (axis + 2) % 3;

  glm::vec3 p = glm::vec3(pos);

  glm::vec3 du(0.0f), dv(0.0f);
  du[u] = size[u];
  dv[v] = size[v];
  glm::vec3 v0 = p;
  glm::vec3 v1 = p + du;
  glm::vec3 v2 = p + du + dv;
  glm::vec3 v3 = p + dv;

  if (backFace)
  {
    if (axis == 0)
    {
      v0 += glm::vec3(step, 0, 0);
      v1 += glm::vec3(step, 0, 0);
      v2 += glm::vec3(step, 0, 0);
      v3 += glm::vec3(step, 0, 0);
    }
    if (axis == 1)
    {
      v0 += glm::vec3(0, step, 0);
      v1 += glm::vec3(0, step, 0);
      v2 += glm::vec3(0, step, 0);
      v3 += glm::vec3(0, step, 0);
    }
    if (axis == 2)
    {
      v0 += glm::vec3(0, 0, step);
      v1 += glm::vec3(0, 0, step);
      v2 += glm::vec3(0, 0, step);
      v3 += glm::vec3(0, 0, step);
    }
  }

  uint32_t start = static_cast<uint32_t>(vertices.size());

  if (axis == 0)
  {
    vertices.push_back({v0, {1, 0}, texture});
    vertices.push_back({v1, {1, 1}, texture});
    vertices.push_back({v2, {0, 1}, texture});
    vertices.push_back({v3, {0, 0}, texture});
  }
  else
  {
    vertices.push_back({v0, {0, 0}, texture});
    vertices.push_back({v1, {1, 0}, texture});
    vertices.push_back({v2, {1, 1}, texture});
    vertices.push_back({v3, {0, 1}, texture});
  }

  bool flip = backFace;
  if (!flip)
  {
    indices.push_back(start + 0);
    indices.push_back(start + 1);
    indices.push_back(start + 2);
    indices.push_back(start + 2);
    indices.push_back(start + 3);
    indices.push_back(start + 0);
  }
  else
  {
    indices.push_back(start + 0);
    indices.push_back(start + 2);
    indices.push_back(start + 1);
    indices.push_back(start + 0);
    indices.push_back(start + 3);
    indices.push_back(start + 2);
  }
}

int Index3D(int x, int y, int z, int w, int l, int h)
{
  return x + w * z + w * l * y;
}

bool IsSolid(const std::vector<Voxel> &voxels, const BlockRegistry &registry, int x, int y, int z, int step, int w, int h, int d)
{
  int sx = x * step;
  int sy = y * step;
  int sz = z * step;

  if (sx < 0 || sy < 0 || sz < 0 ||
      sx >= w || sy >= h || sz >= d)
    return false;

  uint32_t type = voxels[Index3D(sx, sy, sz, w, d, h)].type;
  return registry.blocks[type].visible;
}

void MeshingSystem::Init(std::shared_ptr<Coordinator> coordinator)
{
  gCoordinator = coordinator;
}

void MeshingSystem::Update(Texture voxelTextures, Renderer &renderer)
{
  for (auto &e : mEntities)
  {
    if (!gCoordinator->HasComponent<ChunkComponent>(e))
    {
      continue;
    }

    auto &chunk = gCoordinator->GetComponent<ChunkComponent>(e);
    if (chunk.chunkState == ChunkState::NeedsMeshing)
    {
      CreateMesh(voxelTextures, renderer, e);
      chunk.chunkState = ChunkState::Clean;
    }
  }
}

void MeshingSystem::CreateMesh(Texture voxelTextures, Renderer &renderer, Entity chunkEntity)
{
  auto &chunk = gCoordinator->GetComponent<ChunkComponent>(chunkEntity);
  const int step = 1 << chunk.chunkLOD; // step doubles for each lod

  const int W = world.chunkWidth / step;
  const int H = world.chunkHeight / step;
  const int D = world.chunkLength / step;

  auto &voxels = chunk.voxelData;
  auto &registry = world.registry;

  std::vector<VoxelVertex> vertices;
  std::vector<uint32_t> indices;

  glm::vec3 chunkWorldPos = glm::vec3(chunk.worldPosition);

  for (int axis = 0; axis < 3; axis++)
  {
    int u = (axis + 1) % 3;
    int v = (axis + 2) % 3;

    int dims[3] = {W, H, D};
    std::vector<int> mask(dims[u] * dims[v]);

    for (int d = -1; d < dims[axis]; d++)
    {
      int n = 0;

      for (int j = 0; j < dims[v]; j++)
      {
        for (int i = 0; i < dims[u]; i++)
        {
          int x[3] = {};
          int y[3] = {};

          x[axis] = d;
          y[axis] = d + 1;
          x[u] = y[u] = i;
          x[v] = y[v] = j;

          bool a = IsSolid(voxels, registry, x[0], x[1], x[2], step, world.chunkWidth, world.chunkHeight, world.chunkLength);
          bool b = IsSolid(voxels, registry, y[0], y[1], y[2], step, world.chunkWidth, world.chunkHeight, world.chunkLength);

          if (a == b)
            mask[n++] = 0;
          else
          {
            int idx = a ? Index3D(x[0] * step, x[1] * step, x[2] * step, world.chunkWidth, world.chunkLength, world.chunkHeight) : Index3D(y[0] * step, y[1] * step, y[2] * step, world.chunkWidth, world.chunkLength, world.chunkHeight);

            mask[n++] = a ? (idx + 1) : -(idx + 1);
          }
        }
      }

      n = 0;

      for (int j = 0; j < dims[v]; j++)
      {
        for (int i = 0; i < dims[u];)
        {
          int c = mask[n];
          if (c == 0)
          {
            i++;
            n++;
            continue;
          }

          int w = 1;
          while (i + w < dims[u] && mask[n + w] == c)
            w++;

          int h = 1;
          bool stop = false;
          while (j + h < dims[v] && !stop)
          {
            for (int k = 0; k < w; k++)
            {
              if (mask[n + k + h * dims[u]] != c)
              {
                stop = true;
                break;
              }
            }
            if (!stop)
              h++;
          }

          for (int y2 = 0; y2 < h; y2++)
            for (int x2 = 0; x2 < w; x2++)
              mask[n + x2 + y2 * dims[u]] = 0;

          int voxelIndex = abs(c) - 1;
          uint32_t type = voxels[voxelIndex].type;
          const BlockType &block = registry.blocks[type];

          int tex;
          if (axis == 1)
            tex = (c > 0) ? block.textureBottom : block.textureTop;
          else
            tex = block.textureSide;

          glm::ivec3 pos(0);
          pos[axis] = d + (c > 0);
          pos[u] = i;
          pos[v] = j;

          glm::ivec3 size(0);
          size[u] = w;
          size[v] = h;
          size[axis] = 1;

          EmitQuad(vertices, indices, pos, size, axis, c < 0, tex, step);

          i += w;
          n += w;
        }
      }
    }
  }

  if (gCoordinator->HasComponent<VoxelMeshComponent>(chunkEntity))
  {
    VoxelMeshComponent mesh = gCoordinator->GetComponent<VoxelMeshComponent>(chunkEntity);
    mesh.mesh->Cleanup();
    gCoordinator->RemoveComponent<VoxelMeshComponent>(chunkEntity);
  }

  if (vertices.size() > 0 && indices.size() > 0)
  {
    TransformComponent chunkTransform{};
    chunkTransform.translation = {chunk.worldPosition.x * world.chunkWidth, -chunk.worldPosition.y * world.chunkHeight, chunk.worldPosition.z * world.chunkLength};
    chunkTransform.scale = {1.0f, 1.0f, 1.0f};
    renderer.storageBufferAccess[chunk.gpuIndex].model = chunkTransform.GetMatrix();

    gCoordinator->AddComponent(chunkEntity, chunkTransform);
    auto mesh = std::make_shared<VoxelMesh>(renderer);
    mesh->Init(voxelTextures, vertices, indices);
    gCoordinator->AddComponent(chunkEntity, VoxelMeshComponent{mesh});
  }
}
