#pragma once

#include <cstdint>

struct AssetDirectoryEntry
{
    uint64_t assetID = 0;
    uint32_t typeID = 0;
    uint32_t schemaVersion = 1;
    uint64_t dataOffset = 0;
    uint64_t dataSize = 0;
};