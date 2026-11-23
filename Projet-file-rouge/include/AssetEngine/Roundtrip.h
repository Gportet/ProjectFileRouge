#pragma once
#include "BinaryWriter.h"
#include "BinaryReader.h"
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

class Roundtrip
{
private:
    BinaryWriter writer;
    BinaryReader reader;

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
        const std::string utf8Str  = "Hello,there ! 456 éàü";

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
        // Test 1 : Buffer tronqué (3 octets au lieu des 4 requis pour un U32)
        {
            Bytes truncated = {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}};
            reader = BinaryReader(truncated);

            bool threw = false;
            try { reader.ReadU32(); }
            catch (const std::out_of_range&) { threw = true; }

            if (!threw) throw std::runtime_error("Robustness failed: truncated U32 did not throw");
        }

        // Test 2 : Booléen corrompu (0x02 n'est ni 0x00 ni 0x01)
        {
            Bytes invalidBool = {std::byte{0x02}};
            reader = BinaryReader(invalidBool);

            bool threw = false;
            try { reader.ReadBool(); }
            catch (const std::runtime_error&) { threw = true; }

            if (!threw) throw std::runtime_error("Robustness failed: invalid bool did not throw");
        }

        // Test 3 : Longueur de chaîne mensongère (annonce 1000 octets alors que le buffer s'arrête)
        {
            writer.clear();
            writer.Write(static_cast<std::uint32_t>(1000));
            reader = BinaryReader(writer.getBuffer());

            bool threw = false;
            try { reader.ReadString(); }
            catch (const std::out_of_range&) { threw = true; }

            if (!threw) throw std::runtime_error("Robustness failed: corrupted string length did not throw");
        }
    }
};