#include "modelLoading.hpp"
#include "vertexData.hpp"
#include "components.hpp"

glm::mat4 AiToGlmMat(const aiMatrix4x4 &mat)
{
  return glm::mat4(
      mat.a1, mat.b1, mat.c1, mat.d1,
      mat.a2, mat.b2, mat.c2, mat.d2,
      mat.a3, mat.b3, mat.c3, mat.d3,
      mat.a4, mat.b4, mat.c4, mat.d4);
}

std::shared_ptr<Mesh> LoadAiMesh(Renderer &renderer, aiMesh *mesh)
{
  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++)
  {
    Vertex v{};
    v.pos = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
    // if (mesh->HasNormals())
    // v.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
    if (mesh->HasTextureCoords(0))
      v.texCoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
    vertices.push_back(v);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++)
  {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  auto m = std::make_shared<Mesh>(renderer);
  m->Init(vertices, indices);
  return m;
}

Entity LoadNodeRecursive(std::shared_ptr<Coordinator> coordinator, Renderer &renderer, aiNode *node, const aiScene *scene, Entity parent)
{
  Entity entity = coordinator->CreateEntity();

  // Transform
  glm::mat4 localTransform = AiToGlmMat(node->mTransformation);
  glm::vec3 translation, scale, skew;
  glm::quat rotation;
  glm::vec4 perspective;
  glm::decompose(localTransform, scale, rotation, translation, skew, perspective);
  coordinator->AddComponent(entity, TransformComponent{translation, glm::degrees(glm::eulerAngles(rotation)), scale});

  // Parent/Children
  if (parent != -1)
  {
    if (!coordinator->HasComponent<Parent>(parent))
      coordinator->AddComponent(entity, Parent{entity});

    if (!coordinator->HasComponent<Children>(parent))
      coordinator->AddComponent(parent, Children{});

    coordinator->GetComponent<Children>(parent).values.push_back(entity);
  }

  // Meshes
  for (unsigned int i = 0; i < node->mNumMeshes; i++)
  {
    aiMesh *aiMesh = scene->mMeshes[node->mMeshes[i]];
    coordinator->AddComponent(entity, MeshComponent{LoadAiMesh(renderer, aiMesh)});
  }

  // Recurse
  for (unsigned int i = 0; i < node->mNumChildren; i++)
  {
    LoadNodeRecursive(coordinator, renderer, node->mChildren[i], scene, entity);
  }

  return entity;
}

void LoadModel(Entity parentEntity, std::shared_ptr<Coordinator> coordinator, Renderer &renderer, const std::string &path)
{
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
  if (!scene || !scene->mRootNode)
    throw std::runtime_error("Failed to load model: " + path);

  LoadNodeRecursive(coordinator, renderer, scene->mRootNode, scene, parentEntity);
}