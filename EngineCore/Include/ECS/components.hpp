#pragma once
#include <memory>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "types.hpp"

class Mesh;

struct TransformComponent
{
    glm::vec3 translation{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f}; // in degrees (0-360), not radians
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    glm::mat4 GetMatrix() const
    {
        glm::mat4 mat = glm::mat4(1.0f);
        mat = glm::translate(mat, translation);
        mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        mat = glm::scale(mat, scale);
        return mat;
    }
};

struct MeshComponent
{
    std::shared_ptr<Mesh> mesh;

    MeshComponent() = default;
    MeshComponent(std::shared_ptr<Mesh> m) : mesh(m) {}
};

struct Parent
{
    Entity value = -1;
};

struct Children
{
    std::vector<Entity> values{};
};