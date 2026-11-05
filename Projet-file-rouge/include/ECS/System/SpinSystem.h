#pragma once

#include "ECS/Component/SpinComponent.h"
#include "ECS/Component/TransformComponent.h"
#include "ECS/ComponentManager.h"
#include "ECS/EntityManager.h"

namespace ECS
{

    inline void SpinSystem(EntityManager& em, float dt)
    {
        em.ForAllEntities(MakeMask<TransformComponent, SpinComponent>(), [&](Entity e)
        {
            TransformComponent& transform = componentManager.getComponent<TransformComponent>(e);
            const SpinComponent& spin = componentManager.getComponent<SpinComponent>(e);

            transform.rotation.x += spin.radiansPerSecond.x * dt;
            transform.rotation.y += spin.radiansPerSecond.y * dt;
            transform.rotation.z += spin.radiansPerSecond.z * dt;
        });
    }
}