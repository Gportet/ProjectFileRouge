#pragma once

#include "Shader.h"

class LambertShader :  public Shader
{
public:
    uint32_t ShaderFragment(const MathsSIMD::Vec3_SIMD& cameraPosition, 
    const MathsSIMD::Vec3_SIMD& position, 
    const MathsSIMD::Vec3_SIMD& normals,  
    const DirectionalLight& light, 
    const Vec2& UV, 
    const Material& material) const override; 
};
