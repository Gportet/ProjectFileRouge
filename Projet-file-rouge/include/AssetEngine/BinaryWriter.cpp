#include <algorithm>
#include <fstream>
#include <bit>
#include <ios>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <cstdint>
#include <limits>
#include <iostream>

typedef std::vector<std::byte> Bytes;

class BinaryWriter
{

private:
    Bytes buffer;

public:
    // #TODO : integers, booleans, floating-point values, UTF-8 strings, variable-length sequences and composite structures.
    Bytes getBuffer() const
    {
        return buffer;
    }

    void Write(const std::vector<std::byte>& data)
    {
        buffer.insert(buffer.end(), data.begin(), data.end());
    }

    
    // U64
    void Write(std::uint64_t value) // Little endian
    {
        for (int i = 0; i < 8; ++i)
        {
            buffer.push_back(static_cast<std::byte>(value & 0xFF)); 

            value >>= 8;
        }
    }

    // U32
    void Write(std::uint32_t value) // Little endian
    {
            for (int i = 0; i < 4; ++i)
            {
                buffer.push_back(static_cast<std::byte>(value & 0xFF)); 

                value >>= 8;
            }
    }

    // U16
    void Write(std::uint16_t value) // Little endian
    {
        for (int i = 0; i < 2; ++i)
        {
            buffer.push_back(static_cast<std::byte>(value & 0xFF)); 

            value >>= 8;
        }
    }

    // U8
    void Write(std::uint8_t value)
    {
        buffer.push_back(std::byte{value});
    }

    // Float
    void Write(float value)
    {
        std::uint32_t rawValue = std::bit_cast<std::uint32_t>(value);
        Write(rawValue);
    }

    // Double
    void Write(double value)
    {
        std::uint64_t rawValue = std::bit_cast<std::uint64_t>(value);
        Write(rawValue);
    }

    // Boolean
    void Write(bool value)
    {
        Write(static_cast<std::uint8_t>(value ? 1 : 0));
    }

    // String (UTF-8)
    void Write(std::string_view text)
    {
        if (text.size() > std::numeric_limits<std::uint32_t>::max()){throw std::length_error("String too large");}
            
        Write(static_cast<std::uint32_t>(text.size()));

        const auto *data = reinterpret_cast<const std::byte*>(text.data());
        buffer.insert(buffer.end(), data, data + text.size());
    }

    template <typename T>
    void Write(const std::vector<T>& vec)
    {
        if (vec.size() > std::numeric_limits<std::uint32_t>::max())
            throw std::length_error("Vector too large");

        Write(static_cast<std::uint32_t>(vec.size()));

        for (const T& element : vec)
        {
            Write(element);
        }
    }

    void Save(const std::string& path) const
    {
        std::ofstream file(path, std::ios::binary | std::ios::trunc);

        if (!file)
            throw std::runtime_error("Cannot open file");

        for (std::byte byte : buffer)
        {
            file.put(static_cast<char>(std::to_integer<unsigned char>(byte)));
        }
    }
};

