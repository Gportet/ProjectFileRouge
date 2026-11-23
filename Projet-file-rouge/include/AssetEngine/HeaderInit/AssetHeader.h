#pragma once
#include <cstdint>
#include <array>

#include "../Binary/BinaryTypes.h"

struct AssetHeader
{
    std::array<char, 5> magicNumber = {'S', 'I', 'N', 'G', 'E'};
    std::uint32_t formatVersion = 1;
    std::uint32_t flags = 0;
    std::uint64_t totalFileSize = 0;
    std::uint32_t checksum = 0;
    std::uint32_t assetCount = 0;
    std::uint32_t directoryOffset = 33;
    static constexpr std::uint32_t HeaderSize = 33;
};

Bytes SerializeAssetHeader(const AssetHeader& header);
AssetHeader DeserializeAssetHeader(const Bytes& bytes);
bool isValidAssetHeader(const AssetHeader& header);