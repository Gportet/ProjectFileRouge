
#include "AssetEngine/HeaderInit/AssetHeader.h"

#include "AssetEngine/Binary/BinaryWriter.h"
#include "AssetEngine/Binary/BinaryReader.h"

#include <stdexcept>

Bytes SerializeAssetHeader(const AssetHeader& header)
{
    BinaryWriter writer;
    
    for (char character : header.magicNumber)
        writer.Write(static_cast<std::uint8_t>(character));

    // Header fields
    writer.Write(header.formatVersion);
    writer.Write(header.flags);
    writer.Write(header.totalFileSize);
    writer.Write(header.checksum);
    writer.Write(header.assetCount);
    writer.Write(header.directoryOffset);

    return writer.getBuffer();
}

AssetHeader DeserializeAssetHeader(const Bytes& bytes)
{
    if (bytes.size() < AssetHeader::HeaderSize)
        throw std::runtime_error("Invalid asset header: insufficient data");

    BinaryReader reader(bytes);
    AssetHeader header;

    for (char& character : header.magicNumber)
        character = static_cast<char>(reader.ReadU8());

    header.formatVersion = reader.ReadU32();
    header.flags = reader.ReadU32();
    header.totalFileSize = reader.ReadU64();
    header.checksum = reader.ReadU32();
    header.assetCount = reader.ReadU32();
    header.directoryOffset = reader.ReadU32();

    return header;
}

bool isValidAssetHeader(const AssetHeader& header)
{
    return header.magicNumber == std::array<char, 5>{'S', 'I', 'N', 'G', 'E'}
    && header.totalFileSize >= AssetHeader::HeaderSize
    && header.formatVersion == 1
    && header.directoryOffset == AssetHeader::HeaderSize;
}
