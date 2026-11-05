#include "Engine/Rendering/Shader/BlinnPhongShader.h"


uint32_t BlinnPhongShader::ShaderFragment(const MathsSIMD::Vec3_SIMD& cameraPosition,const MathsSIMD::Vec3_SIMD& position,const MathsSIMD::Vec3_SIMD& normals,const DirectionalLight& light,const Vec2& UV,const Material& material) const
{
    const uint32_t color = material.getTexture()->SampleNearest(UV.u, UV.v);
    // 1. Normalisation des vecteurs
    const MathsSIMD::Vec3_SIMD N = normals.Normalize();
    const MathsSIMD::Vec3_SIMD L = light.direction.Normalize();

    const MathsSIMD::Vec3_SIMD V =(cameraPosition - position).Normalize();

    const MathsSIMD::Vec3_SIMD H = (L + V).Normalize();
    
    // 2. Calcul de la composante diffuse et ambiante
    const auto diffuse = material.getDiffuse() * std::max(0.0f, N.Dot(L));
    const auto ambient = material.getAmbient() * light.ambient;
    
    // 3. Calcul de la composante shinisess
    const auto shininess = material.getShininess();

    
    const float specularFactor = std::pow(std::max(0.0f, N.Dot(H)),shininess);
    
    const auto specular = material.getSpecular() * light.specularStrength * specularFactor;

    const MathsSIMD::Vec4_SIMD intensity = ambient + diffuse + specular;

    // Extraction des composantes
    const float texR = ((color >> 16) & 0xFF) / 255.0f;
    const float texG = ((color >> 8) & 0xFF) / 255.0f;
    const float texB = (color & 0xFF) / 255.0f;
    
    const float r = std::clamp(MathsSIMD::Vec3_SIMD::GetX(intensity.data) * texR, 0.0f, 1.0f);
    const float g = std::clamp(MathsSIMD::Vec3_SIMD::GetY(intensity.data) * texG, 0.0f, 1.0f);
    const float b = std::clamp(MathsSIMD::Vec3_SIMD::GetZ(intensity.data) * texB, 0.0f, 1.0f);
    
    // 6. Application de l'intensité sur les composantes RGB
    const uint8_t r8 = static_cast<uint8_t>(r * 255.0f);
    const uint8_t g8 = static_cast<uint8_t>(g * 255.0f);
    const uint8_t b8 = static_cast<uint8_t>(b * 255.0f);
    const uint8_t a = (color >> 24) & 0xFF;

    // 7. Reconstruction de la couleur ARGB
    return (static_cast<uint32_t>(a) << 24)
         | (static_cast<uint32_t>(r8) << 16)
         | (static_cast<uint32_t>(g8) << 8)
         | static_cast<uint32_t>(b8);
}

