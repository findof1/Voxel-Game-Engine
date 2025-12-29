#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <unordered_map>
#include <memory>

#include "mesh.hpp"
#include "entityManager.hpp"
#include "coordinator.hpp"
#include "texture.hpp"

glm::mat4 AiToGlmMat(const aiMatrix4x4 &mat);
std::shared_ptr<Mesh> LoadAiMesh(Renderer &renderer, aiMesh *mesh, Texture texture);
Entity LoadNodeRecursive(std::shared_ptr<Coordinator> coordinator, Renderer &renderer, aiNode *node, const aiScene *scene, Texture texture, Entity parent = -1);
void LoadModel(Entity parentEntity, std::shared_ptr<Coordinator> coordinator, Renderer &renderer, Texture texture, const std::string &path);
