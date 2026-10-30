#include "Engine/utils/RandomGenerator.h"

namespace
{
    constexpr uint64_t PCG_MULTIPLIER = 6364136223846793005ULL;
}

RandomGenerator::RandomGenerator(uint64_t seed, uint64_t sequence)
{
    m_state = 0;
    m_increment = (sequence << 1u) | 1u;
    
    NextUInt();
    m_state += seed;
    NextUInt();

}

uint32_t RandomGenerator::NextUInt()
{
    const uint64_t oldState = m_state;
 
    // We switch the state for the next call
    m_state = oldState * PCG_MULTIPLIER + m_increment;
    
    // We use XOR to Shuffle the bits
    const uint32_t xorshifted = static_cast<uint32_t>(((oldState >> 18u) ^ oldState) >> 27u);
    
    // We rotate with oldState to have a pseudo random
    const uint32_t rot = static_cast<uint32_t>((oldState >> 59u));
        
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

float RandomGenerator::NextFloat()
{
    return static_cast<float>(NextUInt() >> 8u) * (1.0f / 16777216.0f);
}
