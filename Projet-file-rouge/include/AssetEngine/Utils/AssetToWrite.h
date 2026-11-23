#pragma once

#include <cstdint>
#include "AssetEngine/Binary/BinaryTypes.h"

struct AssetToWrite
{
    uint64_t assetID = 0;
    uint32_t typeID = 0;
    uint32_t schemaVersion = 1;

    Bytes data;
};