#pragma once

#include <filesystem>
#include <vector>

#include "AssetEngine/HeaderInit/AssetHeader.h"
#include "AssetEngine/Binary/BinaryTypes.h"
#include "AssetEngine/Utils/AssetDirectory.h"

class AssetFileReader
{
public:
    void Read(
        const std::filesystem::path& path,
        AssetHeader& header,
        std::vector<AssetDirectoryEntry>& directory,
        std::vector<Bytes>& payloads
    );
};
