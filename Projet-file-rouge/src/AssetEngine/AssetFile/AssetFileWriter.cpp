
#include "AssetEngine/AssetFile/AssetFileWriter.h"
#include "AssetEngine/Utils/AssetDirectory.h"
#include "AssetEngine/Utils/CheckSum.h"
#include "AssetEngine/Binary/BinaryWriter.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <limits>
#include <unordered_set>
#include <cstdint>
#include <cstddef>

void AssetFileWriter::Write(
    const std::filesystem::path& path,
    AssetHeader header,
    const std::vector<AssetToWrite>& assets)
{
    constexpr uint32_t MaxAssets = 100;
    constexpr uint64_t DirectoryEntrySize = 32;

    // 1. Vérifier le nombre d'assets.
    if (assets.size() > MaxAssets)
        throw std::runtime_error("Too many assets");

    // 2. Vérifier les IDs et calculer la taille totale des payloads.
    std::unordered_set<uint64_t> assetIDs;

    uint64_t totalDataSize = 0;

    for (const auto& asset : assets)
    {
        if (asset.assetID == 0)
            throw std::runtime_error("Invalid asset ID");

        if (!assetIDs.insert(asset.assetID).second)
            throw std::runtime_error("Duplicate asset ID");

        if (asset.data.size() >
            std::numeric_limits<uint64_t>::max() - totalDataSize)
        {
            throw std::runtime_error("Payload size overflow");
        }

        totalDataSize += static_cast<uint64_t>(asset.data.size());
    }

    // 3. Calculer la taille de la Directory.
    const uint64_t directorySize =
        static_cast<uint64_t>(assets.size()) * DirectoryEntrySize;

    // 4. Calculer le début des payloads.
    const uint64_t payloadStart =
        AssetHeader::HeaderSize + directorySize;

    // 5. Calculer la taille totale du fichier.
    if (totalDataSize >
        std::numeric_limits<uint64_t>::max() - payloadStart)
    {
        throw std::runtime_error("File size overflow");
    }

    const uint64_t totalFileSize = payloadStart + totalDataSize;

    if (totalFileSize >
        static_cast<uint64_t>(
            std::numeric_limits<std::streamsize>::max()))
    {
        throw std::runtime_error("File is too big for streamsize");
    }

    if (totalFileSize >
        static_cast<uint64_t>(
            std::numeric_limits<size_t>::max()))
    {
        throw std::runtime_error("File is too big for memory allocation");
    }

    // 6. Mettre à jour le Header.
    header.totalFileSize = totalFileSize;
    header.assetCount = static_cast<uint32_t>(assets.size());
    header.directoryOffset = AssetHeader::HeaderSize;
    header.checksum = 0;

    // 7. Construire la Directory.
    BinaryWriter directoryWriter;

    uint64_t currentOffset = payloadStart;

    for (const auto& asset : assets)
    {
        AssetDirectoryEntry entry;

        entry.assetID = asset.assetID;
        entry.typeID = asset.typeID;
        entry.schemaVersion = asset.schemaVersion;
        entry.dataOffset = currentOffset;
        entry.dataSize = static_cast<uint64_t>(asset.data.size());

        directoryWriter.Write(entry.assetID);
        directoryWriter.Write(entry.typeID);
        directoryWriter.Write(entry.schemaVersion);
        directoryWriter.Write(entry.dataOffset);
        directoryWriter.Write(entry.dataSize);

        currentOffset += entry.dataSize;
    }

    Bytes directoryData = directoryWriter.getBuffer();

    if (directoryData.size() != directorySize)
        throw std::runtime_error("Invalid directory size");

    // 8. Sérialiser le Header avec un checksum à zéro.
    Bytes headerBytes = SerializeAssetHeader(header);

    if (headerBytes.size() != AssetHeader::HeaderSize)
        throw std::runtime_error("Invalid serialized header size");

    // 9. Assembler l'archive complète.
    Bytes archive;

    archive.reserve(static_cast<size_t>(totalFileSize));

    archive.insert(
        archive.end(),
        headerBytes.begin(),
        headerBytes.end()
    );

    archive.insert(
        archive.end(),
        directoryData.begin(),
        directoryData.end()
    );

    for (const auto& asset : assets)
    {
        archive.insert(
            archive.end(),
            asset.data.begin(),
            asset.data.end()
        );
    }

    if (archive.size() != totalFileSize)
        throw std::runtime_error("Invalid archive size");

    // 10. Calculer le CRC32 sur toute l'archive.
    // Le champ checksum est encore à zéro.
    header.checksum = CalculateChecksum(archive);

    // 11. Réécrire le Header avec le checksum calculé.
    headerBytes = SerializeAssetHeader(header);

    if (headerBytes.size() != AssetHeader::HeaderSize)
        throw std::runtime_error("Invalid serialized header size");

    std::copy(
        headerBytes.begin(),
        headerBytes.end(),
        archive.begin()
    );

    // 12. Écrire l'archive sur le disque.
    std::ofstream file(
        path,
        std::ios::binary | std::ios::trunc
    );

    if (!file.is_open())
        throw std::runtime_error("Unable to open asset file");

    file.write(
        reinterpret_cast<const char*>(archive.data()),
        static_cast<std::streamsize>(archive.size())
    );

    if (!file)
        throw std::runtime_error("Failed to write asset file");

    file.close();

    if (file.fail())
        throw std::runtime_error("Failed to close asset file");
}
