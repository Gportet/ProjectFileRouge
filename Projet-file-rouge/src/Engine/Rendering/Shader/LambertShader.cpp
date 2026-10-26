#include "Engine/Rendering/Shader/LambertShader.h"

uint32_t LambertShader::ShaderFragment(const MathsSIMD::Vec3_SIMD& cameraPosition, const MathsSIMD::Vec3_SIMD& position, const MathsSIMD::Vec3_SIMD& normals, const DirectionalLight& light, const Vec2& UV, const Texture& texture) const
{
    // 1. Récupération de la couleur de la texture
    const uint32_t color = texture.SampleNearest(UV.u, UV.v);

    // 2. Normalisation des vecteurs
    const auto N = normals.Normalize();
    const auto L = light.direction.Normalize();

    // 3. Calcul de l'éclairage diffus Lambert
    const float diffuse = std::max(0.0f, N.Dot(L));

    // 4. Ajout de la lumière ambiante
    const float intensity = std::min(1.0f, light.ambient + diffuse);

    // 5. Application de l'intensité sur les composantes RGB
    const uint8_t r = static_cast<uint8_t>(((color >> 16) & 0xFF) * intensity);
    const uint8_t g = static_cast<uint8_t>(((color >> 8) & 0xFF) * intensity);
    const uint8_t b = static_cast<uint8_t>((color & 0xFF) * intensity);
    const uint8_t a = (color >> 24) & 0xFF;

    // 6. Reconstruction de la couleur ARGB
    return (static_cast<uint32_t>(a) << 24)
         | (static_cast<uint32_t>(r) << 16)
         | (static_cast<uint32_t>(g) << 8)
         | static_cast<uint32_t>(b);
}
