#pragma once
#include "BinaryReader.h"
#include <cstdint>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct ValidatedAssetEntry
{
    std::uint64_t id = 0;
    std::uint32_t typeId = 0;
    std::uint32_t schemaVersion = 0;
    std::uint64_t dataOffset = 0;
    std::uint64_t dataSize = 0;
};

struct ValidatedArchiveLayout
{
    std::uint32_t formatVersion = 0;
    std::uint32_t flags = 0;
    std::uint32_t storedChecksum = 0;
    std::unordered_map<std::uint64_t, ValidatedAssetEntry> directory;
};

class ArchiveValidator
{
public:
    static constexpr std::uint32_t HeaderSize = 33;
    static constexpr std::uint32_t DirectoryEntrySize = 32;
    static constexpr std::uint32_t SupportedFormatVersion = 1;
    static constexpr std::uint32_t MaxAssets = 100;

    // Vérification anti-overflow de plage [offset, offset + size]
    static bool IsValidRange(std::uint64_t offset, std::uint64_t size, std::uint64_t totalSize)
    {
        return offset <= totalSize && size <= (totalSize - offset);
    }

    // Valide la structure complète d'une archive (Header + Directory) avant exploitation
    static ValidatedArchiveLayout ValidateBuffer(const Bytes& rawBuffer)
    {
        const std::uint64_t fileSize = rawBuffer.size();

        // 1. Fichier tronqué avant la fin du Header (32 octets)
        if (fileSize < HeaderSize)
        {
            throw std::out_of_range("Invalid archive: file smaller than 32-byte header");
        }

        BinaryReader reader(rawBuffer);

        // 2. Magic Number ("SINGE")
        const char m0 = static_cast<char>(reader.ReadU8());
        const char m1 = static_cast<char>(reader.ReadU8());
        const char m2 = static_cast<char>(reader.ReadU8());
        const char m3 = static_cast<char>(reader.ReadU8());
        const char m4 = static_cast<char>(reader.ReadU8());
        if (m0 != 'S' || m1 != 'I' || m2 != 'N' || m3 != 'G' || m4 != 'E')
        {
            throw std::runtime_error("Invalid archive: bad magic number");
        }

        // 3. Rejet explicite des versions inconnues
        const std::uint32_t formatVersion = reader.ReadU32();
        if (formatVersion != SupportedFormatVersion)
        {
            throw std::runtime_error("Invalid archive: unsupported format version");
        }

        const std::uint32_t flags = reader.ReadU32();

        // 4. Cohérence entre la taille déclarée et la taille réelle du buffer
        const std::uint64_t declaredTotalSize = reader.ReadU64();
        if (declaredTotalSize != fileSize)
        {
            throw std::runtime_error("Invalid archive: total file size mismatch");
        }

        // 5. Lecture du champ Checksum (vérification déléguée au module d'intégrité du binôme)
        const std::uint32_t storedChecksum = reader.ReadU32();

        const std::uint32_t assetCount = reader.ReadU32();
        const std::uint32_t directoryOffset = reader.ReadU32();

        // 6. Limitation d'allocation et bornes du Directory
        if (assetCount > MaxAssets)
        {
            throw std::length_error("Invalid archive: asset count exceeds maximum allowed");
        }

        if (directoryOffset != HeaderSize)
        {
            throw std::runtime_error("Invalid archive: unexpected directory offset");
        }

        const std::uint64_t directoryByteSize = static_cast<std::uint64_t>(assetCount) * DirectoryEntrySize;
        if (!IsValidRange(directoryOffset, directoryByteSize, fileSize))
        {
            throw std::out_of_range("Invalid archive: directory extends beyond file size");
        }

        const std::uint64_t payloadsStartOffset = directoryOffset + directoryByteSize;

        // 7. Lecture et validation du Directory dans un objet temporaire (atomicité)
        ValidatedArchiveLayout tempLayout;
        tempLayout.formatVersion = formatVersion;
        tempLayout.flags = flags;
        tempLayout.storedChecksum = storedChecksum;
        tempLayout.directory.reserve(assetCount);

        for (std::uint32_t i = 0; i < assetCount; ++i)
        {
            ValidatedAssetEntry entry;
            entry.id            = reader.ReadU64();
            entry.typeId        = reader.ReadU32();
            entry.schemaVersion = reader.ReadU32();
            entry.dataOffset    = reader.ReadU64();
            entry.dataSize      = reader.ReadU64();

            // Détection des IDs dupliqués
            if (tempLayout.directory.contains(entry.id))
            {
                throw std::runtime_error("Invalid archive: duplicate Asset ID detected");
            }

            // Validation anti-overflow des offsets et tailles de payload
            if (entry.dataOffset < payloadsStartOffset || !IsValidRange(entry.dataOffset, entry.dataSize, fileSize))
            {
                throw std::out_of_range("Invalid archive: asset payload offset or size out of bounds");
            }

            tempLayout.directory.emplace(entry.id, entry);
        }

        return tempLayout;
    }

    // 8. Vérification des références entre objets (ex: joints physiques -> corps existants)
    static void ValidateReferences(
        const std::unordered_set<std::uint64_t>& existingIds,
        const std::vector<std::uint64_t>& referencedIds,
        std::uint64_t nullId = 0)
    {
        for (std::uint64_t refId : referencedIds)
        {
            if (refId == nullId)
            {
                continue;
            }

            if (!existingIds.contains(refId))
            {
                throw std::runtime_error("Invalid reference: referenced ID does not exist");
            }
        }
    }

    // 9. Restauration atomique : garantit que targetState n'est jamais modifié en cas d'échec
    template <typename StateType, typename LoaderFunc>
    static void AtomicRestore(StateType& targetState, LoaderFunc&& loadIntoTemp)
    {
        StateType tempState;
        loadIntoTemp(tempState);
        targetState = std::move(tempState);
    }
};