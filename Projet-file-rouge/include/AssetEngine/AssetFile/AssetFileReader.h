#pragma once

#include <filesystem>
#include "AssetEngine/HeaderInit/AssetHeader.h"
#include "AssetEngine/Binary/BinaryTypes.h"

class AssetFileReader
{
public:
    void Read(
      const std::filesystem::path& path,
      AssetHeader& header,
      Bytes& data
  );  
};
