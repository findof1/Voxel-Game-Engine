#pragma once
#include <memory>
#include <utility>
#include <functional>

#include "coordinator.hpp"
#include "types.hpp"
#include "components.hpp"
#include "camera.hpp"
class RenderSystem;
class Renderer;
class RenderSystem : public System
{
public:
    std::shared_ptr<Coordinator> gCoordinator;
    int screenWidth;
    int screenHeight;

    void Init(std::shared_ptr<Coordinator> coordinator, int screenWidth, int screenHeight);
    void Update(Renderer &renderer, float deltaTime, const Camera &camera);
    void RenderScene(Renderer &renderer, float deltaTime, const Camera &camera);
};