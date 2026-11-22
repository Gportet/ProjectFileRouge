#pragma once
#include <cstdint>
#include <array>

#include "../Binary/BinaryTypes.h"

struct AssetHeader
{
    std::array<std::uint8_t, 5> magicNumber = {'S', 'I', 'N', 'G', 'E'}; // SINGE
    std::uint32_t version = 1;
    std::uint32_t flags = 0;
    std::uint32_t assetType = 0;
    std::uint32_t fileSize = 0; // Header + Data
    std::uint32_t checksum = 0; 
    static constexpr std::uint32_t HeaderSize = 25; // 25 octets au total
    
    
};

Bytes SerializeAssetHeader(const AssetHeader& header);
AssetHeader DeserializeAssetHeader(const Bytes& bytes);
bool isValidAssetHeader(const AssetHeader& header);