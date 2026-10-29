#pragma once
#include <cstdint>

#include "Engine/Light.h"
#include "Engine/Mesh.h"
#include "Engine/Ressources/Texture.h"
#include "Engine/Ressources/Material.h"
#include "MathLib_SIMD/include/Vec3_SIMD.h"

class Shader
{
public:
    virtual  ~Shader() = default;
    virtual uint32_t ShaderFragment(const MathsSIMD::Vec3_SIMD& cameraPosition, 
        const MathsSIMD::Vec3_SIMD& position, 
        const MathsSIMD::Vec3_SIMD& normals,  
        const DirectionalLight& light, 
        const Vec2& UV, 
        const Material& material) const = 0;
};

