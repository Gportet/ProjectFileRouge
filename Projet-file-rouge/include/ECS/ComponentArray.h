# pragma once
#include <array>
#include <vector>
#include <cassert>
#include "Entity.h"


namespace ECS
{

    class IComponentArray {                
    public:
        virtual ~IComponentArray() = default;
        virtual void    Add(Entity e, void* component) = 0;
        virtual void    Remove(Entity e) = 0;
        virtual bool    Has(Entity e) const = 0;
        virtual void*   Get(Entity e) = 0;
    };

    template <class T>
    class ComponentArray : public IComponentArray {
    private:
        std::array<T, MAX_ENTITIES>      data{};
        std::array<bool, MAX_ENTITIES>   owned{};

    public:
        void Add(Entity e, void* component) override
        {
            assert(e < MAX_ENTITIES);
            owned[e] = true;
            data[e] = *static_cast<T*>(component);
        }
        void Remove(Entity e) override
        {
            assert(Has(e));
            owned[e] = false;
            data[e] = T{};
        }
        bool Has(Entity e) const override
        {
            return e < MAX_ENTITIES && owned[e];
        }
        void* Get(Entity e) override
        {
            assert(Has(e));
            return static_cast<void*>(&data[e]);
        }
    };
}