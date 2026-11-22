#include "AssetEngine/AssetFile/AssetFileReader.h"

#include <fstream>
#include <stdexcept>
#include <vector>


void AssetFileReader::Read(const std::filesystem::path& path, AssetHeader& header, Bytes& data)
{
    // 1. Ouvrir le fichier en lecture binaire
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Unable to open asset file");

    // 2. Déterminer la taille réelle du fichier
    file.seekg(0, std::ios::end);
    const std::streamsize fileSize = file.tellg();

    if (fileSize < static_cast<std::streamsize>(AssetHeader::HeaderSize))
        throw std::runtime_error("Asset file is too small");

    file.seekg(0, std::ios::beg);

    // 3. Lire les 25 octets du header
    Bytes headerBytes(AssetHeader::HeaderSize);

    file.read(
        reinterpret_cast<char*>(headerBytes.data()),
        static_cast<std::streamsize>(headerBytes.size())
    );

    if (!file)
        throw std::runtime_error("Failed to read asset header");

    // 4. Désérialiser et vérifier le header
    header = DeserializeAssetHeader(headerBytes);

    if (!isValidAssetHeader(header))
        throw std::runtime_error("Invalid asset header");

    // 5. Vérifier que la taille annoncée correspond au fichier réel
    if (header.fileSize != static_cast<std::uint64_t>(fileSize))
        throw std::runtime_error("Asset file size mismatch");

    // 6. Lire les données après le header
    const std::size_t dataSize = header.fileSize - AssetHeader::HeaderSize;

    data.resize(dataSize);

    if (!data.empty())
    {
        file.read(
            reinterpret_cast<char*>(data.data()),
            static_cast<std::streamsize>(data.size())
        );

        if (!file)
            throw std::runtime_error("Failed to read asset data");
    }
}
