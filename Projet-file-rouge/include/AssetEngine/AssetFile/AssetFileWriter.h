#pragma once

#include <filesystem>
#include "AssetEngine/HeaderInit/AssetHeader.h"
#include "AssetEngine/Binary/BinaryTypes.h"

class AssetFileWriter
{
public:
    void Write(
      const std::filesystem::path& path,
      AssetHeader header,
      const Bytes& data
  );  
};
