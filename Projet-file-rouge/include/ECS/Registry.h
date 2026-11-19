#pragma once
#include <functional>
#include <utility>
#include <vector>

#include "ECS/ComponentManager.h"
#include "ECS/EntityManager.h"

namespace ECS
{
    using System = std::function<void(EntityManager&)>;

    class Registry {
        public:
        EntityManager em;
        std::vector<System> systems;

        EntityManager& GetEntityManager() {
            return em;
        }


        Entity CreateEntity()
        {
            return em.CreateEntity();
        }

        void DestroyEntity(Entity e)
        {
            componentManager.EntityDestroyed(e);   
            em.DestroyEntity(e);                    
        }


        template<typename T>
        void RegisterComponent()
        {
            componentManager.addComponentType<T>();
        }

        template<typename T>
        void AddComponent(Entity e, T component)
        {
            componentManager.addComponent<T>(e, std::move(component));
            em.AddComponent(e, static_cast<int>(GetTypeId<T>()));
        }

        template<typename T>
        void RemoveComponent(Entity e)
        {
            componentManager.removeComponent<T>(e);
            em.RemoveComponent(e, static_cast<int>(GetTypeId<T>()));
        }

        template<typename T>
        bool HasComponent(Entity e)
        {
            return componentManager.hasComponent<T>(e);
        }

        template<typename T>
        T& GetComponent(Entity e)
        {
            return componentManager.getComponent<T>(e);
        }


        void AddSystem(System system)
        {
            systems.push_back(std::move(system));
        }

        void Update()
        {
            for (auto& s : systems)
            {
                s(em);
            }
        }
    };
}