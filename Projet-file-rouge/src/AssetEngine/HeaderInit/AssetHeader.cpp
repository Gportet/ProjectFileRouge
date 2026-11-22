#include "AssetEngine/HeaderInit/AssetHeader.h"

#include "AssetEngine/Binary/BinaryWriter.h"
#include "AssetEngine/Binary/BinaryReader.h"


Bytes SerializeAssetHeader(const AssetHeader& header)
{
    BinaryWriter writer;
    for (std::size_t magicIndex = 0; magicIndex < header.magicNumber.size(); ++magicIndex)
    {
        writer.Write(header.magicNumber[magicIndex]);
    }
    writer.Write(header.version);
    writer.Write(header.flags);
    writer.Write(header.assetType);
    writer.Write(header.fileSize);
    writer.Write(header.checksum);
    
    return writer.getBuffer();
}


AssetHeader DeserializeAssetHeader(const Bytes& bytes)
{
    BinaryReader reader(bytes);
    AssetHeader header;
    for (std::size_t magicIndex = 0; magicIndex < header.magicNumber.size(); ++magicIndex)
    {
        header.magicNumber[magicIndex] = reader.ReadU8();
    }

    header.version = reader.ReadU32();
    header.flags = reader.ReadU32();
    header.assetType = reader.ReadU32();
    header.fileSize = reader.ReadU32();
    header.checksum = reader.ReadU32();

    return header;
}

bool isValidAssetHeader(const AssetHeader& header)
{
    
}
