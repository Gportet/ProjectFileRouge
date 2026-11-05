#pragma once
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "ECS/ComponentArray.h"

inline std::size_t NextTypeId()
{
    static std::size_t counter = 0;
    return counter++;
}

template<typename T>
std::size_t GetTypeId()
{
    static const std::size_t id = NextTypeId();
    return id;
}

namespace ECS
{
    class ComponentManager
    {
    private:
        std::vector<IComponentArray*> componentsPool;

        template<typename T>
        IComponentArray& getArray()
        {
            const std::size_t id = GetTypeId<T>();
            assert(id < componentsPool.size() && componentsPool[id] != nullptr);
            return *componentsPool[id];
        }
    public:

        ComponentManager() = default;
        ComponentManager(const ComponentManager&) = delete;
        ComponentManager& operator=(const ComponentManager&) = delete;

        ~ComponentManager()
        {
            for (IComponentArray* component : componentsPool)
                delete component;
        }

        template<typename T>
        std::size_t addComponentType()
        {
            const std::size_t id = GetTypeId<T>();
            if (id >= componentsPool.size())
                componentsPool.resize(id + 1, nullptr);
            assert(componentsPool[id] == nullptr);

            componentsPool[id] = new ComponentArray<T>();
            return id;
        }

        template<typename T>
        void addComponent(Entity e, T c)
        {
            getArray<T>().Add(e, &c);
        }

        template<typename T>
        void removeComponent(Entity e)
        {
            getArray<T>().Remove(e);
        }

        template<typename T>
        bool hasComponent(Entity e)
        {
            return getArray<T>().Has(e);
        }

        template<typename T>
        T& getComponent(Entity e)
        {
            return *static_cast<T*>(getArray<T>().Get(e));
        }

        void EntityDestroyed(Entity e)
        {
            for (IComponentArray* component : componentsPool)
            {
                if (component != nullptr && component->Has(e))
                    component->Remove(e);
            }
        }
    };

    // Masque de recherche pour EntityManager::ForAllEntities : un bit par type de composant.
    // Exemple : MakeMask<TransformComponent, MeshComponent>()
    template<typename... Ts>
    std::uint32_t MakeMask()
    {
        return ((1u << GetTypeId<Ts>()) | ...);
    }
}

// instance unique 
extern ECS::ComponentManager componentManager;