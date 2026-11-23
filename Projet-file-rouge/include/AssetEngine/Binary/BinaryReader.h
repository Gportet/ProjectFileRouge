#include <algorithm>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <cstdint>
#include <limits>
#include <iostream>

#include "BinaryTypes.h"

class BinaryReader
{

private:
    Bytes buffer;
    std::size_t offset = 0;

public:
    BinaryReader() = default;

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
        else if constexpr (std::is_same_v<T, float>)         return ReadFloat();
        else if constexpr (std::is_same_v<T, double>)        return ReadDouble();
        else if constexpr (std::is_same_v<T, bool>)          return ReadBool();
        else if constexpr (std::is_same_v<T, std::string>)   return ReadString();
        else static_assert(false, "Type non supporté par BinaryReader::Read<T>");
    }


    void readFromFile(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);

        if (!file)
            throw std::runtime_error("Cannot open file");

        buffer.clear();

        std::byte byte;
        while (file.read(reinterpret_cast<char*>(&byte), sizeof(byte)))
        {
            buffer.push_back(byte);
        }
    }

    std::uint8_t ReadU8()
    {
        if (offset + 1 > buffer.size()) throw std::out_of_range("EOF");
        return std::to_integer<std::uint8_t>(buffer[offset++]);
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

    std::string ReadString()
    {
        std::uint32_t length = ReadU32(); 
        if (length > buffer.size() - offset){throw std::out_of_range("EOF / Corrupted length");}
            

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
        if (length > buffer.size() - offset){throw std::out_of_range("EOF / Corrupted length");}
        std::vector<T> result;
        result.reserve(length);
        for (std::uint32_t i = 0; i < length; ++i)
        {
            result.push_back(Read<T>());
        }
        return result;
    }

    bool ReadBool()
    {
        std::uint8_t value = ReadU8();
        if (value > 1) throw std::runtime_error("Invalid boolean value");
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

    Bytes ReadBytes(std::size_t count)
    {
        if (offset + count > buffer.size()) throw std::out_of_range("EOF");
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
    
    
    [[nodiscard]] bool isAtEnd() const
    {
        return offset == buffer.size();
    }
    
};
