#pragma once
#include <cstdint>

namespace ECS
{
    using Entity = std::uint32_t;
    
    inline constexpr Entity MAX_ENTITIES = 1000;
}