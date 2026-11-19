#pragma once

class Pipeline;

namespace ECS
{
    class EntityManager;

    // pipeline
    void RenderSystem(EntityManager& em, Pipeline& pipeline);
}