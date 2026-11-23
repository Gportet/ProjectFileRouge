
#include "AssetEngine/AssetFile/AssetFileReader.h"
#include "AssetEngine/Utils/AssetDirectory.h"
#include "AssetEngine/Utils/CheckSum.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

namespace
{
    // Lit un entier stocké en Little-Endian.
    template<typename T>
    T ReadLittleEndian(const Bytes& bytes, std::size_t& offset)
    {
        static_assert(std::is_unsigned_v<T>);

        if (offset > bytes.size() ||
            sizeof(T) > bytes.size() - offset)
        {
            throw std::runtime_error("Unexpected end of archive");
        }

        T value = 0;

        for (std::size_t i = 0; i < sizeof(T); ++i)
        {
            const auto byte =
                std::to_integer<std::uint8_t>(bytes[offset + i]);

            value |= static_cast<T>(byte) << (i * 8);
        }

        offset += sizeof(T);
        return value;
    }
}

void AssetFileReader::Read(
    const std::filesystem::path& path,
    AssetHeader& header,
    std::vector<AssetDirectoryEntry>& directory,
    std::vector<Bytes>& payloads)
{
    constexpr std::uint64_t DirectoryEntrySize = 32;
    constexpr std::uint32_t MaxAssets = 100;
    constexpr std::size_t ChecksumOffset = 21;

    // 1. Ouvrir le fichier en lecture binaire.
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Unable to open asset file");

    // 2. Déterminer la taille réelle du fichier.
    file.seekg(0, std::ios::end);

    const std::streamsize fileSize = file.tellg();

    if (fileSize < static_cast<std::streamsize>(AssetHeader::HeaderSize))
        throw std::runtime_error("Asset file is too small");

    file.seekg(0, std::ios::beg);

    // 3. Lire l'archive complète.
    Bytes archive(static_cast<std::size_t>(fileSize));

    file.read(
        reinterpret_cast<char*>(archive.data()),
        fileSize
    );

    if (!file)
        throw std::runtime_error("Failed to read asset file");

    // 4. Lire et désérialiser le Header.
    Bytes headerBytes(
        archive.begin(),
        archive.begin() + AssetHeader::HeaderSize
    );

    header = DeserializeAssetHeader(headerBytes);

    if (!isValidAssetHeader(header))
        throw std::runtime_error("Invalid asset header");

    // 5. Vérifier la taille annoncée.
    if (header.totalFileSize != archive.size())
        throw std::runtime_error("Asset file size mismatch");

    // 6. Vérifier le nombre d'assets.
    if (header.assetCount > MaxAssets)
        throw std::runtime_error("Too many assets");

    if (header.directoryOffset != AssetHeader::HeaderSize)
        throw std::runtime_error("Invalid directory offset");

    // 7. Calculer la fin de la Directory.
    const std::uint64_t directorySize =
        static_cast<std::uint64_t>(header.assetCount) *
        DirectoryEntrySize;

    const std::uint64_t directoryEnd =
        header.directoryOffset + directorySize;

    if (directoryEnd > archive.size())
        throw std::runtime_error("Directory exceeds archive size");

    // 8. Vérifier le CRC32 sur toute l'archive.
    // On remet temporairement le champ checksum à zéro.
    Bytes checksumBuffer = archive;

    for (std::size_t i = 0; i < sizeof(std::uint32_t); ++i)
        checksumBuffer[ChecksumOffset + i] = std::byte{0};

    const std::uint32_t calculatedChecksum =
        CalculateChecksum(checksumBuffer);

    if (calculatedChecksum != header.checksum)
        throw std::runtime_error("Asset checksum mismatch");

    // 9. Lire les entrées de la Directory.
    std::vector<AssetDirectoryEntry> newDirectory;
    std::vector<Bytes> newPayloads;

    newDirectory.reserve(header.assetCount);
    newPayloads.reserve(header.assetCount);

    std::unordered_set<std::uint64_t> assetIDs;

    std::size_t offset = header.directoryOffset;

    for (std::uint32_t i = 0; i < header.assetCount; ++i)
    {
        AssetDirectoryEntry entry;

        entry.assetID = ReadLittleEndian<std::uint64_t>(archive, offset);
        entry.typeID = ReadLittleEndian<std::uint32_t>(archive, offset);
        entry.schemaVersion = ReadLittleEndian<std::uint32_t>(archive, offset);
        entry.dataOffset = ReadLittleEndian<std::uint64_t>(archive, offset);
        entry.dataSize = ReadLittleEndian<std::uint64_t>(archive, offset);

        // Vérifier l'identifiant.
        if (entry.assetID == 0)
            throw std::runtime_error("Invalid asset ID");

        if (!assetIDs.insert(entry.assetID).second)
            throw std::runtime_error("Duplicate asset ID");

        // Vérifier que le payload ne chevauche pas le Header ou la Directory.
        if (entry.dataOffset < directoryEnd)
            throw std::runtime_error("Payload overlaps archive metadata");

        // Vérifier que le payload reste dans les limites du fichier.
        if (entry.dataOffset > archive.size() ||
            entry.dataSize > archive.size() - entry.dataOffset)
        {
            throw std::runtime_error("Invalid payload range");
        }

        newDirectory.push_back(entry);
    }

    // 10. Vérifier que les payloads ne se chevauchent pas.
    std::vector<std::pair<std::uint64_t, std::uint64_t>> ranges;

    for (const auto& entry : newDirectory)
    {
        ranges.emplace_back(
            entry.dataOffset,
            entry.dataOffset + entry.dataSize
        );
    }

    std::sort(ranges.begin(), ranges.end());

    for (std::size_t i = 1; i < ranges.size(); ++i)
    {
        if (ranges[i].first < ranges[i - 1].second)
            throw std::runtime_error("Payloads overlap");
    }

    // 11. Extraire chaque payload.
    for (const auto& entry : newDirectory)
    {
        const auto begin = archive.begin() +
            static_cast<std::ptrdiff_t>(entry.dataOffset);

        const auto end = begin +
            static_cast<std::ptrdiff_t>(entry.dataSize);

        newPayloads.emplace_back(begin, end);
    }

    // 12. Transférer les résultats vers les paramètres.
    directory = std::move(newDirectory);
    payloads = std::move(newPayloads);
}
