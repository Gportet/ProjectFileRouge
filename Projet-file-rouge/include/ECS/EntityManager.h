#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <vector>

#include "ECS/Entity.h"

namespace ECS
{
    class EntityManager {

        private:
            Entity                                   nextId = 0;
            std::vector<Entity>                      freeIds;
            std::array<std::uint32_t, MAX_ENTITIES>  entityComponents{};

        public:
            Entity CreateEntity()
            {
                if (!freeIds.empty())
                {
                    const Entity e = freeIds.back();
                    freeIds.pop_back();
                    return e;
                }
                assert(nextId < MAX_ENTITIES);
                return nextId++;
            }

            void DestroyEntity(Entity e)
            {
                assert(e < nextId);
                freeIds.push_back(e);
                entityComponents[e] = 0;
            }

            void AddComponent(Entity e, int componentID)
            {
                assert(e < nextId);
                assert(componentID >= 0 && componentID < 32);  
                entityComponents[e] |= (1u << componentID);
            }

            void RemoveComponent(Entity e, int componentID)
            {
                assert(e < nextId);
                entityComponents[e] &= ~(1u << componentID);
            }

            // Appelle func(e) pour chaque entité qui possède AU MOINS tous les bits de searchMask.
            // (searchMask == 0 correspondrait à toutes les entités, y compris les détruites)
            template<typename Func>
            void ForAllEntities(std::uint32_t searchMask, Func&& func)
            {
                for (Entity e = 0; e < nextId; e++)
                {
                    if ((entityComponents[e] & searchMask) == searchMask)
                    {
                        func(e);
                    }
                }
            }
    };
}