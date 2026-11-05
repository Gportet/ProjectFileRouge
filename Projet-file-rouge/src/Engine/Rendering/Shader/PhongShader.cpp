#include "Engine/Rendering/Shader/PhongShader.h"

uint32_t PhongShader::ShaderFragment(const MathsSIMD::Vec3_SIMD& cameraPosition, const MathsSIMD::Vec3_SIMD& position, const MathsSIMD::Vec3_SIMD& normals, const DirectionalLight& light, const Vec2& UV, const Material& material) const
{
    // 1. Récupération de la couleur de la texture
    const uint32_t color = material.getTexture()->SampleNearest(UV.u, UV.v);

    
    // 2. Normalisation des vecteurs
    const auto N = normals.Normalize();
    const auto L = light.direction.Normalize();
    const auto V = (cameraPosition - position).Normalize();

    // 3. Calcul de l'éclairage diffus Lambert
    const auto diffuse = material.getDiffuse() * std::max(0.0f, N.Dot(L));

    // 4. Calcul de la lumière ambiante//
    const auto ambient = material.getAmbient() * light.ambient;

    // 5. Calcul de la composante spéculaire Phong
    const float NdotL = N.Dot(L);
    const auto R = (N * (2.0f * NdotL) - L).Normalize();

    const auto specular = material.getSpecular() * light.specularStrength *  pow32(std::max(0.0f, R.Dot(V)));

    const MathsSIMD::Vec4_SIMD intensity = ambient + diffuse + specular;

    // Extraction des composantes
    const float texR = ((color >> 16) & 0xFF) / 255.0f;
    const float texG = ((color >> 8) & 0xFF) / 255.0f;
    const float texB = (color & 0xFF) / 255.0f;
    
    const float r = std::clamp(MathsSIMD::Vec3_SIMD::GetX(intensity.data) * texR, 0.0f, 1.0f);
    const float g = std::clamp(MathsSIMD::Vec3_SIMD::GetY(intensity.data) * texG, 0.0f, 1.0f);
    const float b = std::clamp(MathsSIMD::Vec3_SIMD::GetZ(intensity.data) * texB, 0.0f, 1.0f);

    // 7. Application de l'intensité sur les composantes RGB
    const uint8_t r8 = static_cast<uint8_t>(r * 255.0f);
    const uint8_t g8 = static_cast<uint8_t>(g * 255.0f);
    const uint8_t b8 = static_cast<uint8_t>(b * 255.0f);
    const uint8_t a = (color >> 24) & 0xFF;

    // 8. Reconstruction de la couleur ARGB
    return (static_cast<uint32_t>(a) << 24)
         | (static_cast<uint32_t>(r8) << 16)
         | (static_cast<uint32_t>(g8) << 8)
         | static_cast<uint32_t>(b8);
    
}





