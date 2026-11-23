#pragma once
#include "Utils/ArchiveValidator.h"
#include "Binary/BinaryReader.h"
#include "Binary/BinaryWriter.h"
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include "AssetEngine/AssetFile/AssetFileWriter.h"
#include "AssetEngine/AssetFile/AssetFileReader.h"

#include <filesystem>
#include <fstream>


#include "AssetEngine/Serializer/TextureSerializer.h"
#include "AssetEngine/Serializer/MaterialSerializer.h"

#include "Engine/Ressources/Texture.h"
#include "Engine/Ressources/Material.h"

#include <memory>
#include <iostream>
#include <array>

class Roundtrip
{
private:
    BinaryWriter writer;
    BinaryReader reader;

    // Utilitaire générique pour vérifier qu'une action lève bien l'exception attendue
    template <typename ExceptionType, typename Func>
    void expectThrow(Func&& action, const std::string& errorMessage)
    {
        bool threw = false;
        try
        {
            action();
        }
        catch (const ExceptionType&)
        {
            threw = true;
        }

        if (!threw)
        {
            throw std::runtime_error("Robustness failed: " + errorMessage);
        }
    }

    // Construit un buffer d'archive complet (Header 32B + Directory + Payloads) pour les tests
    Bytes buildMockArchive(
        const std::string& magic,
        std::uint32_t version,
        const std::vector<ValidatedAssetEntry>& entries,
        const Bytes& payloadData,
        bool overrideTotalSize = false,
        std::uint64_t customTotalSize = 0)
    {
        writer.clear();

        const std::uint32_t assetCount = static_cast<std::uint32_t>(entries.size());
        const std::uint32_t dirOffset = 33;
        const std::uint64_t computedTotalSize = 33ULL + (entries.size() * 32ULL) + payloadData.size();
        const std::uint64_t totalSize = overrideTotalSize ? customTotalSize : computedTotalSize;

        // 1. Header (33 octets)
        for (int i = 0; i < 5; ++i)
        {
            writer.Write(static_cast<std::uint8_t>(i < static_cast<int>(magic.size()) ? magic[i] : 0));
        }
        writer.Write(version);
        writer.Write(static_cast<std::uint32_t>(0)); // flags
        writer.Write(totalSize);
        writer.Write(static_cast<std::uint32_t>(0)); // checksum (géré par le binôme)
        writer.Write(assetCount);
        writer.Write(dirOffset);

        // 2. Directory (N * 32 octets)
        for (const auto& entry : entries)
        {
            writer.Write(entry.id);
            writer.Write(entry.typeId);
            writer.Write(entry.schemaVersion);
            writer.Write(entry.dataOffset);
            writer.Write(entry.dataSize);
        }

        // 3. Payloads
        writer.Write(payloadData);

        return writer.getBuffer();
    }

public:
    Roundtrip() = default;

    void verifyAll()
    {
        writer = BinaryWriter();
        reader = BinaryReader();

        verifyPrimitives();
        verifyStringsAndCollections();
        verifyRobustness();
    }

private:
    void verifyPrimitives()
    {
        writer.clear();

        const bool boolTrue  = true;
        const bool boolFalse = false;

        const std::uint8_t  u8Val  = 250;
        const std::uint16_t u16Val = 65000;
        const std::uint32_t u32Val = 4000000000U;
        const std::uint64_t u64Val = 18000000000000000000ULL;

        const std::int8_t   i8Val  = -128;
        const std::int16_t  i16Val = -32768;
        const std::int32_t  i32Val = -2147483647 - 1;
        const std::int64_t  i64Val = -9223372036854775807LL - 1;

        const float  floatVal  = -3.14159f;
        const double doubleVal = 3.141592653589793;

        writer.Write(boolTrue);
        writer.Write(boolFalse);
        writer.Write(u8Val);
        writer.Write(u16Val);
        writer.Write(u32Val);
        writer.Write(u64Val);
        writer.Write(i8Val);
        writer.Write(i16Val);
        writer.Write(i32Val);
        writer.Write(i64Val);
        writer.Write(floatVal);
        writer.Write(doubleVal);

        reader = BinaryReader(writer.getBuffer());

        if (reader.ReadBool()   != boolTrue)  throw std::runtime_error("Bool (true) mismatch");
        if (reader.ReadBool()   != boolFalse) throw std::runtime_error("Bool (false) mismatch");
        if (reader.ReadU8()     != u8Val)     throw std::runtime_error("U8 mismatch");
        if (reader.ReadU16()    != u16Val)    throw std::runtime_error("U16 mismatch");
        if (reader.ReadU32()    != u32Val)    throw std::runtime_error("U32 mismatch");
        if (reader.ReadU64()    != u64Val)    throw std::runtime_error("U64 mismatch");
        if (reader.ReadI8()     != i8Val)     throw std::runtime_error("I8 mismatch");
        if (reader.ReadI16()    != i16Val)    throw std::runtime_error("I16 mismatch");
        if (reader.ReadI32()    != i32Val)    throw std::runtime_error("I32 mismatch");
        if (reader.ReadI64()    != i64Val)    throw std::runtime_error("I64 mismatch");
        if (reader.ReadFloat()  != floatVal)  throw std::runtime_error("Float mismatch");
        if (reader.ReadDouble() != doubleVal) throw std::runtime_error("Double mismatch");
    }

    void verifyStringsAndCollections()
    {
        writer.clear();

        const std::string emptyStr = "";
        const std::string utf8Str  = "Hello, Moteur ! 456 éàü";

        const std::vector<std::uint32_t> emptyVec  = {};
        const std::vector<std::int32_t>  signedVec = {-100, -1, 0, 42, 9999};
        const std::vector<float>         floatVec  = {0.0f, -1.5f, 3.14f, 42.25f};
        const std::vector<std::string>   stringVec = {"Header", "Directory", "Payload", ""};

        writer.Write(emptyStr);
        writer.Write(utf8Str);
        writer.Write(emptyVec);
        writer.Write(signedVec);
        writer.Write(floatVec);
        writer.Write(stringVec);

        reader = BinaryReader(writer.getBuffer());

        if (reader.ReadString() != emptyStr) throw std::runtime_error("Empty string mismatch");
        if (reader.ReadString() != utf8Str)  throw std::runtime_error("UTF-8 string mismatch");

        if (reader.ReadVector<std::uint32_t>() != emptyVec)  throw std::runtime_error("Empty vector mismatch");
        if (reader.ReadVector<std::int32_t>()  != signedVec) throw std::runtime_error("Signed vector mismatch");
        if (reader.ReadVector<float>()         != floatVec)  throw std::runtime_error("Float vector mismatch");
        if (reader.ReadVector<std::string>()   != stringVec) throw std::runtime_error("String vector mismatch");
    }

  void verifyRobustness()
    {
        // 0. Cas valide de contrôle (vérifie qu'une archive bien formée passe sans erreur)
        {
            Bytes validPayload(16, std::byte{0xBB});
            // Header (33) + 2 entrées (64) = payloads démarrant à l'offset 97
            std::vector<ValidatedAssetEntry> validEntries = {
                {10, 1, 1, 97, 8},
                {20, 2, 1, 105, 8}
            };
            Bytes validArchive = buildMockArchive("SINGE", 1, validEntries, validPayload);
            ValidatedArchiveLayout layout = ArchiveValidator::ValidateBuffer(validArchive);
            if (layout.directory.size() != 2 || !layout.directory.contains(10) || !layout.directory.contains(20))
            {
                throw std::runtime_error("Robustness failed: valid archive was not parsed properly");
            }
        }

        // 1. Primitives tronquées (3 octets au lieu de 4 pour U32)
        reader = BinaryReader({std::byte{0x01}, std::byte{0x02}, std::byte{0x03}});
        expectThrow<std::out_of_range>(
            [&]() { reader.ReadU32(); },
            "truncated U32 did not throw"
        );

        // 2. Booléen invalide (valeur 0x02 interdite)
        reader = BinaryReader({std::byte{0x02}});
        expectThrow<std::runtime_error>(
            [&]() { reader.ReadBool(); },
            "invalid bool did not throw"
        );

        // 3. Longueur de chaîne dépassant le buffer (tronquée)
        writer.clear();
        writer.Write(static_cast<std::uint32_t>(1000));
        reader = BinaryReader(writer.getBuffer());
        expectThrow<std::out_of_range>(
            [&]() { reader.ReadString(); },
            "truncated string length did not throw"
        );

        // 4. Limitation des allocations (demande 100M de uint64_t > 64 Mio)
        writer.clear();
        writer.Write(static_cast<std::uint32_t>(100'000'000));
        reader = BinaryReader(writer.getBuffer());
        expectThrow<std::length_error>(
            [&]() { reader.ReadVector<std::uint64_t>(); },
            "excessive vector allocation did not throw"
        );

        // 5. Archive tronquée (< 33 octets)
        Bytes tinyArchive(16, std::byte{0x00});
        expectThrow<std::out_of_range>(
            [&]() { ArchiveValidator::ValidateBuffer(tinyArchive); },
            "archive smaller than header did not throw"
        );

        // 6. Mauvais Magic Number ("FAKEE" au lieu de "SINGE")
        Bytes badMagic = buildMockArchive("FAKEE", 1, {}, {});
        expectThrow<std::runtime_error>(
            [&]() { ArchiveValidator::ValidateBuffer(badMagic); },
            "bad magic number did not throw"
        );

        // 7. Version du format inconnue (version 99)
        Bytes badVersion = buildMockArchive("SINGE", 99, {}, {});
        expectThrow<std::runtime_error>(
            [&]() { ArchiveValidator::ValidateBuffer(badVersion); },
            "unknown format version did not throw"
        );

        // 8. Taille déclarée dans le header incohérente avec le fichier réel
        Bytes badTotalSize = buildMockArchive("SINGE", 1, {}, {}, true, 9999);
        expectThrow<std::runtime_error>(
            [&]() { ArchiveValidator::ValidateBuffer(badTotalSize); },
            "mismatched total file size did not throw"
        );

        // 9. IDs dupliqués dans le Directory
        Bytes payload(16, std::byte{0xAA});
        // Header (33) + 2 entrées (64) = payloads démarrant à l'offset 97
        std::vector<ValidatedAssetEntry> dupEntries = {
            {42, 1, 1, 97, 8},
            {42, 1, 1, 105, 8} // Même ID = 42 !
        };
        Bytes dupArchive = buildMockArchive("SINGE", 1, dupEntries, payload);
        expectThrow<std::runtime_error>(
            [&]() { ArchiveValidator::ValidateBuffer(dupArchive); },
            "duplicate Asset ID did not throw"
        );

        // 10. Dépassement d'entier (Integer Overflow) sur offset + size
        std::vector<ValidatedAssetEntry> overflowEntries = {
            {1, 1, 1, std::numeric_limits<std::uint64_t>::max() - 5, 100}
        };
        Bytes overflowArchive = buildMockArchive("SINGE", 1, overflowEntries, {});
        expectThrow<std::out_of_range>(
            [&]() { ArchiveValidator::ValidateBuffer(overflowArchive); },
            "offset + size integer overflow did not throw"
        );

        // 11. Payload qui empiète sur le Header/Directory (offset = 16 < 65)
        std::vector<ValidatedAssetEntry> overlapEntries = {
            {1, 1, 1, 16, 8}
        };
        Bytes overlapArchive = buildMockArchive("SINGE", 1, overlapEntries, payload);
        expectThrow<std::out_of_range>(
            [&]() { ArchiveValidator::ValidateBuffer(overlapArchive); },
            "payload overlapping header/directory did not throw"
        );

        // 12. Référence invalide (ID cible 999 inexistant)
        const std::unordered_set<std::uint64_t> validIds = {1, 2, 3};
        const std::vector<std::uint64_t> brokenRefs = {1, 999};
        expectThrow<std::runtime_error>(
            [&]() { ArchiveValidator::ValidateReferences(validIds, brokenRefs); },
            "invalid reference ID did not throw"
        );

        // 13. Échec d'écriture sur disque (chemin invalide)
        writer.clear();
        writer.Write(static_cast<std::uint32_t>(123));
        expectThrow<std::runtime_error>(
            [&]() { writer.Save("/dossier_inexistant_xyz/archive.bin"); },
            "write failure to invalid path did not throw"
        );

        // 14. Atomicité : aucun état partiellement restauré après erreur
        std::vector<std::uint32_t> activeWorldState = {10, 20, 30};
        expectThrow<std::runtime_error>(
            [&]()
            {
                ArchiveValidator::AtomicRestore(activeWorldState, [](std::vector<std::uint32_t>& tempState)
                {
                    tempState.push_back(999); // Modification partielle en cours...
                    throw std::runtime_error("Simulated mid-load corruption");
                });
            },
            "atomic restore did not propagate error"
        );
        if (activeWorldState != std::vector<std::uint32_t>{10, 20, 30})
        {
            throw std::runtime_error("Robustness failed: state was partially modified after error");
        }
    }
    
    
};