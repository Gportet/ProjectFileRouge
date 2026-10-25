#pragma once

#include <vector>
#include "Engine/Mesh.h" 
#include "MathLib/MathLib_SIMD/include/Vec4_SIMD.h"

class Pipeline;

enum class ParticleType
{
    Snow,
    firework
};

struct Particle
{
    Vec2 position{0.0f, 0.0f};
    Vec2 velocity{0.0f, 0.0f};
    MathsSIMD::Vec4_SIMD color;
    float lifetime{0.0f};
};


class ParticleGenerator
{
public:
    ParticleGenerator(unsigned int amount);

    void Update(float dt, ParticleType type, Vec2 emitterPos = {0.0f, 0.0f}, unsigned int newParticles = 2);
    void Draw(Pipeline& pipeline);

private:
    std::vector<Particle> m_particles;
    unsigned int m_amount;
    unsigned int m_lastUsedParticle{0};

    unsigned int firstUnusedParticle();
    void respawnParticle(Particle& particle, 
                         ParticleType type, 
                         Vec2 emitterPos, 
                         int maxWIDTH, 
                         int maxHEIGHT);
};