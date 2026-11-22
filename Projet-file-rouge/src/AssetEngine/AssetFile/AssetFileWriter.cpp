#include "AssetEngine/AssetFile/AssetFileWriter.h"

#include <fstream>
#include <stdexcept>

void AssetFileWriter::Write(const std::filesystem::path& path, AssetHeader header, const Bytes& data)
{
    if (data.size() > UINT32_MAX - AssetHeader::HeaderSize)
    {
        throw std::runtime_error("File is too big");
    }

    // Vérifier la taille compatible avec ofstream::write
    if (data.size() > static_cast<std::size_t>(
            std::numeric_limits<std::streamsize>::max()))
        throw std::runtime_error("Data is too big for streamsize");
    
    header.fileSize = AssetHeader::HeaderSize + data.size();
    
    Bytes headerBytes = SerializeAssetHeader(header);

    if (headerBytes.size() != AssetHeader::HeaderSize)
        throw std::runtime_error("Invalid serialized header size");
    
    std::ofstream file(path,std::ios::binary | std::ios::trunc);
    if (!file.is_open())
        throw std::runtime_error("Unable to open asset file");
    
    file.write(reinterpret_cast<const char*>(headerBytes.data()),
        static_cast<std::streamsize>(headerBytes.size()));
    
    if (!data.empty())
    {
        file.write(
            reinterpret_cast<const char*>(data.data()),
            static_cast<std::streamsize>(data.size())
        );
    }
    
    if (!file)
    {
        throw std::runtime_error("Failed to write asset file");
    }

    file.close();
    if (file.fail()) {
        throw std::runtime_error("Failed to close asset file");
    }
}
