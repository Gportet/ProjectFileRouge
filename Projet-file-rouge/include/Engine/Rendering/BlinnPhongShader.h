#pragma once

#include "Engine/Rendering/Shader.h"

class BlinnPhongShader : public Shader
{
public:
    uint32_t ShaderFragment(const MathsSIMD::Vec3_SIMD& cameraPosition, 
    const MathsSIMD::Vec3_SIMD& position, 
    const MathsSIMD::Vec3_SIMD& normals,  
    const DirectionalLight& light, 
    const Vec2& UV, 
    const Texture& texture) const override; 
};
