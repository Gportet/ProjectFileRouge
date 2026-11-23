#pragma once

#include <cstdint>

enum class AssetType : std::uint32_t
{
    Unknown  = 0,
    Mesh     = 1,
    Texture  = 2,
    Material = 3
};