#include "Engine/ParticleGenerator.h"
#include "Engine/Pipeline.h"
#include <cstdlib>



ParticleGenerator::ParticleGenerator(unsigned int amount)
    : m_amount(amount)
{
    m_particles.resize(amount);
}

unsigned int ParticleGenerator::firstUnusedParticle()
{
    for (unsigned int i = m_lastUsedParticle; i < m_amount; ++i)
    {
        if (m_particles[i].lifetime <= 0.0f)
        {
            m_lastUsedParticle = i;
            return i;
        }
    }

    for (unsigned int i = 0; i < m_lastUsedParticle; ++i)
    {
        if (m_particles[i].lifetime <= 0.0f)
        {
            m_lastUsedParticle = i;
            return i;
        }
    }

    m_lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle& particle, 
                                        ParticleType type, 
                                        Vec2 emitterPos, 
                                        int maxWIDTH, 
                                        int maxHEIGHT)
{
    switch (type)
    {
    case ParticleType::Snow:
        {
            float randomU = static_cast<float>(rand() % maxWIDTH);
            float randomV = static_cast<float>(rand() % maxHEIGHT);
                        
            particle.position = { randomU, randomV };
            
            float velU = static_cast<float>((rand() % 40) - 20) / 10.0f; 
            float velV = static_cast<float>((rand() % 50) + 20) / 10.0f; 
            particle.velocity = { velU, velV };
            
            particle.color = MathsSIMD::Vec4_SIMD(_mm_setr_ps(1.0f, 1.0f, 1.0f, 1.0f));
            particle.lifetime = 5.0f; 
            break;
        }
    }
}

    void ParticleGenerator::Update(float dt, ParticleType type, Vec2 emitterPos, unsigned int newParticles)
    {
        for (unsigned int i = 0; i < newParticles; ++i)
        {
            unsigned int unused = firstUnusedParticle();
            respawnParticle(m_particles[unused], type, emitterPos, WIDTH, HEIGHT);
        }
    
        for (auto& p : m_particles)
        {
            p.lifetime -= dt;
            if (p.lifetime > 0.0f)
            {
                p.position.u += p.velocity.u * dt;
                p.position.v += p.velocity.v * dt;
            }
        }
    }

void ParticleGenerator::Draw(Pipeline& pipeline)
{
    for (const auto& p : m_particles)
    {
        if (p.lifetime > 0.0f)
        {
            int px = static_cast<int>(p.position.u);
            int py = static_cast<int>(p.position.v);

            alignas(16) float rawColor[4];
            _mm_store_ps(rawColor, p.color.data);

            uint8_t r = static_cast<uint8_t>(rawColor[0] * 255.0f);
            uint8_t g = static_cast<uint8_t>(rawColor[1] * 255.0f);
            uint8_t b = static_cast<uint8_t>(rawColor[2] * 255.0f);
            uint8_t a = static_cast<uint8_t>(rawColor[3] * 255.0f);


            for (int dy = 0; dy < 2; ++dy)
            {
                for (int dx = 0; dx < 2; ++dx)
                {
                    pipeline.PutPixel(px + dx, py + dy, r, g, b, a);
                }
            }
        }
    }
}