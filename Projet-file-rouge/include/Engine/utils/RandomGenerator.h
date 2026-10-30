#pragma once
#include <cstdint>

class RandomGenerator
{
private:
uint64_t m_state = 0;
uint64_t m_increment = 0;

public:
    explicit RandomGenerator(uint64_t seed, uint64_t sequence = 1);
    
    uint32_t NextUInt();
    float NextFloat();
};
