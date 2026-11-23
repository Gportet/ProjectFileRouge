#pragma once

#include <filesystem>
#include <vector>

#include "AssetEngine/HeaderInit/AssetHeader.h"
#include "AssetEngine/Binary/BinaryTypes.h"
#include "AssetEngine/Utils/AssetToWrite.h"

class AssetFileWriter
{
public:
    void Write(
        const std::filesystem::path& path,
        AssetHeader header,
        const std::vector<AssetToWrite>& assets
    );
};
