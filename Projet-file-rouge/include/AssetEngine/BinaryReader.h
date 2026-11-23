#pragma once
#include <algorithm>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>
#include <bitset>
#include <bit>


typedef std::vector<std::byte> Bytes;
    

class BinaryReader
{

private:
    Bytes buffer;
    std::size_t offset = 0;

public:
    BinaryReader() = default;
    
    // Maximum autorisé allocation dynamique
    static constexpr std::size_t MaxAllocationBytes = 64 * 1024 * 1024;

    explicit BinaryReader(const Bytes& data)
        : buffer(data)
    {
    }
    template <typename T>
    T Read()
    {
        if constexpr (std::is_same_v<T, std::uint8_t>)       return ReadU8();
        else if constexpr (std::is_same_v<T, std::uint16_t>) return ReadU16();
        else if constexpr (std::is_same_v<T, std::uint32_t>) return ReadU32();
        else if constexpr (std::is_same_v<T, std::uint64_t>) return ReadU64();
        else if constexpr (std::is_same_v<T, std::int8_t>)   return ReadI8();
        else if constexpr (std::is_same_v<T, std::int16_t>)  return ReadI16();
        else if constexpr (std::is_same_v<T, std::int32_t>)  return ReadI32();
        else if constexpr (std::is_same_v<T, std::int64_t>)  return ReadI64();
        else if constexpr (std::is_same_v<T, float>)         return ReadFloat();
        else if constexpr (std::is_same_v<T, double>)        return ReadDouble();
        else if constexpr (std::is_same_v<T, bool>)          return ReadBool();
        else if constexpr (std::is_same_v<T, std::string>)   return ReadString();
        else static_assert(false, "Type not supported");
    }
    
    void readFromFile(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);

        if (!file)
        {
            throw std::runtime_error("Cannot open file");
        }
        
        buffer.clear();
        offset = 0;

        std::byte byte;
        while (file.read(reinterpret_cast<char*>(&byte), sizeof(byte)))
        {
            buffer.push_back(byte);
        }
    }

    std::uint8_t ReadU8()
    {
        if (offset + 1 > buffer.size()) throw std::out_of_range("EOF");
        {
            return std::to_integer<std::uint8_t>(buffer[offset++]);
        }
        
    }

    std::uint16_t ReadU16()
    {
        if (offset + 2 > buffer.size()) throw std::out_of_range("EOF");
        std::uint16_t value = 0;
        for (int i = 0; i < 2; ++i)
        {
            value |= static_cast<std::uint16_t>(std::to_integer<std::uint8_t>(buffer[offset++])) << (i * 8);
        }
        return value;
    }

    std::uint32_t ReadU32()
    {
        if (offset + 4 > buffer.size()) throw std::out_of_range("EOF");
        std::uint32_t value = 0;
        for (int i = 0; i < 4; ++i)
        {
            value |= static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(buffer[offset++])) << (i * 8);
        }
        return value;
    }

    std::uint64_t ReadU64()
    {
        if (offset + 8 > buffer.size()) throw std::out_of_range("EOF");
        std::uint64_t value = 0;
        for (int i = 0; i < 8; ++i)
        {
            value |= static_cast<std::uint64_t>(std::to_integer<std::uint8_t>(buffer[offset++])) << (i * 8);
        }
        return value;
    }

    std::int8_t  ReadI8()  { return static_cast<std::int8_t>(ReadU8()); }
    std::int16_t ReadI16() { return static_cast<std::int16_t>(ReadU16()); }
    std::int32_t ReadI32() { return static_cast<std::int32_t>(ReadU32()); }
    std::int64_t ReadI64() { return static_cast<std::int64_t>(ReadU64()); }
    
    
    bool ReadBool()
    {
        std::uint8_t value = ReadU8();
        if (value > 1)
        {
            throw std::runtime_error("Invalid boolean value");
        }
        return value == 1;
    }

    float ReadFloat()
    {
        std::uint32_t rawValue = ReadU32();
        return std::bit_cast<float>(rawValue);
    }

    double ReadDouble()
    {
        std::uint64_t rawValue = ReadU64();
        return std::bit_cast<double>(rawValue);
    }

    std::string ReadString()
    {
        std::uint32_t length = ReadU32();
        if (length > MaxAllocationBytes)
        {
            throw std::length_error("Allocation limit exceeded for string");
        }
            
        if (length > buffer.size() - offset)
        {
            throw std::out_of_range("EOF / Corrupted string length");
        }

        std::string result;
        result.reserve(length);
        for (std::uint32_t i = 0; i < length; ++i)
        {
            result.push_back(static_cast<char>(ReadU8()));
        }
        return result;
    }

    template <typename T>
    std::vector<T> ReadVector()
    {
        std::uint32_t length = ReadU32();
        const std::size_t maxElements = MaxAllocationBytes / sizeof(T);
        if (length > maxElements)
        {
            throw std::length_error("Allocation limit exceeded for vector");
        }
        if (length > buffer.size() - offset)
        {
            throw std::out_of_range("EOF / Corrupted vector length");
        }
        
        std::vector<T> result;
        result.reserve(length);
        for (std::uint32_t i = 0; i < length; ++i)
        {
            result.push_back(Read<T>());
        }
        return result;
    }

    Bytes ReadBytes(std::size_t count)
    {
        if (count > MaxAllocationBytes)
            throw std::length_error("Allocation limit exceeded for raw bytes");
        if (count > buffer.size() - offset)
            throw std::out_of_range("EOF / Requested bytes exceed remaining buffer");

        Bytes result(buffer.begin() + offset, buffer.begin() + offset + count);
        offset += count;
        return result;
    }
    
    void clear()
    {
        buffer.clear();
        offset = 0;
    }
    void reset()
    {
        offset = 0;
    }
};
