#include "Engine/Rendering/Shader/LambertShader.h"

uint32_t LambertShader::ShaderFragment(const MathsSIMD::Vec3_SIMD& cameraPosition, const MathsSIMD::Vec3_SIMD& position, const MathsSIMD::Vec3_SIMD& normals, const DirectionalLight& light, const Vec2& UV, const Material& material) const
{
    // 1. Récupération de la couleur de la texture
    const uint32_t color = material.getTexture()->SampleNearest(UV.u, UV.v);

    // 2. Normalisation des vecteurs
    const auto N = normals.Normalize();
    const auto L = light.direction.Normalize();

    // 3. Calcul de l'éclairage diffus Lambert
    const float diffuseFactor = std::max(0.0f, N.Dot(L));
    const auto diffuse = material.getDiffuse() * diffuseFactor;

    // 4. Calcul de la lumière ambiante
    const auto ambient = material.getAmbient() * light.ambient;
    
    // 5. Combinaison des contributions lumineuses
    const auto intensity = ambient + diffuse;

    // 6. Extraction de la couleur de la texture (normalisation 0-1)
    const float texR = ((color >> 16) & 0xFF) / 255.0f;
    const float texG = ((color >> 8) & 0xFF) / 255.0f;
    const float texB = (color & 0xFF) / 255.0f;

    // 7. Extraction des composantes lumineuses et application de la texture
    const float r = std::clamp(MathsSIMD::Vec3_SIMD::GetX(intensity.data) * texR, 0.0f, 1.0f);
    const float g = std::clamp(MathsSIMD::Vec3_SIMD::GetY(intensity.data) * texG, 0.0f, 1.0f);
    const float b = std::clamp(MathsSIMD::Vec3_SIMD::GetZ(intensity.data) * texB, 0.0f, 1.0f);

    // 8. Conversion en uint8_t
    const uint8_t r8 = static_cast<uint8_t>(r * 255.0f);
    const uint8_t g8 = static_cast<uint8_t>(g * 255.0f);
    const uint8_t b8 = static_cast<uint8_t>(b * 255.0f);
    const uint8_t a = (color >> 24) & 0xFF;

    // 9. Reconstruction de la couleur ARGB
    return (static_cast<uint32_t>(a) << 24)
         | (static_cast<uint32_t>(r8) << 16)
         | (static_cast<uint32_t>(g8) << 8)
         | static_cast<uint32_t>(b8);
}
