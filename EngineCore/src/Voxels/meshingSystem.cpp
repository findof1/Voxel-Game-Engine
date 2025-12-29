#include "meshingSystem.hpp"

void EmitQuad(std::vector<Vertex> &vertices, std::vector<uint16_t> &indices, const glm::ivec3 &pos, const glm::ivec3 &size, int axis, bool backFace, int texture)
{
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
      v0 += glm::vec3(1, 0, 0);
      v1 += glm::vec3(1, 0, 0);
      v2 += glm::vec3(1, 0, 0);
      v3 += glm::vec3(1, 0, 0);
    }
    if (axis == 1)
    {
      v0 += glm::vec3(0, 1, 0);
      v1 += glm::vec3(0, 1, 0);
      v2 += glm::vec3(0, 1, 0);
      v3 += glm::vec3(0, 1, 0);
    }
    if (axis == 2)
    {
      v0 += glm::vec3(0, 0, 1);
      v1 += glm::vec3(0, 0, 1);
      v2 += glm::vec3(0, 0, 1);
      v3 += glm::vec3(0, 0, 1);
    }
  }

  uint32_t start = static_cast<uint32_t>(vertices.size());

  vertices.push_back({v0, {1, 1, 1}, {0, 0}});
  vertices.push_back({v1, {1, 1, 1}, {1, 0}});
  vertices.push_back({v2, {1, 1, 1}, {1, 1}});
  vertices.push_back({v3, {1, 1, 1}, {0, 1}});

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

int Index3D(int x, int y, int z, int w, int l)
{
  return x + w * (z + l * y);
}

bool IsSolid(const std::vector<Voxel> &voxels, const BlockRegistry &registry, int x, int y, int z, int w, int h, int d)
{
  if (x < 0 || y < 0 || z < 0 ||
      x >= w || y >= h || z >= d)
    return false;

  uint32_t type = voxels[Index3D(x, y, z, w, h)].type;
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

  const int W = world.chunkWidth;
  const int H = world.chunkHeight;
  const int D = world.chunkLength;

  auto &voxels = chunk.voxelData;
  auto &registry = world.registry;

  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;

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

          bool a = IsSolid(voxels, registry, x[0], x[1], x[2], W, H, D);
          bool b = IsSolid(voxels, registry, y[0], y[1], y[2], W, H, D);

          if (a == b)
            mask[n++] = 0;
          else
          {
            int idx = a ? Index3D(x[0], x[1], x[2], W, D) : Index3D(y[0], y[1], y[2], W, D);

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
            tex = (c > 0) ? block.textureTop : block.textureBottom;
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

          EmitQuad(vertices, indices, pos, size, axis, c < 0, tex);

          i += w;
          n += w;
        }
      }
    }
  }

  if (gCoordinator->HasComponent<MeshComponent>(chunkEntity))
  {
    MeshComponent mesh = gCoordinator->GetComponent<MeshComponent>(chunkEntity);
    mesh.mesh->Cleanup();
    gCoordinator->RemoveComponent<MeshComponent>(chunkEntity);
  }

  TransformComponent chunkTransform{};
  chunkTransform.translation = {chunk.worldPosition.x * world.chunkWidth, -chunk.worldPosition.y * world.chunkHeight, chunk.worldPosition.z * world.chunkLength};
  chunkTransform.scale = {1.0f, 1.0f, 1.0f};
  gCoordinator->AddComponent(chunkEntity, chunkTransform);
  auto mesh = std::make_shared<Mesh>(renderer);
  mesh->Init(voxelTextures, vertices, indices);
  gCoordinator->AddComponent(chunkEntity, MeshComponent{mesh});
}
