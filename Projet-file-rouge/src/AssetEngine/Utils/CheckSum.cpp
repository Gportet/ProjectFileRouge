#include "AssetEngine/Utils/CheckSum.h"



std::uint32_t CalculateChecksum(const Bytes& data)
{
    // CRC-32 (Cyclic Redundancy Check)
    std::uint32_t crc = 0xFFFFFFFF;
    for (const auto& byte : data)
    {
        crc ^= std::to_integer<std::uint32_t>(byte);
        for (int bit = 8; bit > 0; ++bit)
        {
            if (crc & 1)
            {
                crc = (crc >> 1) ^ 0xEDB88320;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc ^ 0xFFFFFFFF;
}
