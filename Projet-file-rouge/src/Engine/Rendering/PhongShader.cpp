#include "Engine/Rendering/PhongShader.h"

uint32_t PhongShader::ShaderFragment(const MathsSIMD::Vec3_SIMD& cameraPosition, const MathsSIMD::Vec3_SIMD& position, const MathsSIMD::Vec3_SIMD& normals, const DirectionalLight& light, const Vec2& UV, const Texture& texture) const
{
    // 1. Récupération de la couleur de la texture
    const uint32_t color = texture.SampleNearest(UV.u, UV.v);

    // 2. Normalisation des vecteurs
    const auto N = normals.Normalize();
    const auto L = light.direction.Normalize();
    const auto V = (cameraPosition - position).Normalize();

    // 3. Calcul de l'éclairage diffus Lambert
    const float diffuse = std::max(0.0f, N.Dot(L));

    // 4. Calcul de la lumière ambiante
    const float ambient = light.ambient;

    // 5. Calcul de la composante spéculaire Phong
    const float NdotL = N.Dot(L);
    const auto R = (N * (2.0f * NdotL) - L).Normalize();

    const float specular = light.specularStrength *
        pow32(std::max(0.0f, R.Dot(V)));

    // 6. Combinaison des composantes lumineuses
    const float intensity = std::clamp(
        ambient + diffuse + specular,
        0.0f,
        1.0f
    );

    // 7. Application de l'intensité sur les composantes RGB
    const uint8_t r = static_cast<uint8_t>(((color >> 16) & 0xFF) * intensity);
    const uint8_t g = static_cast<uint8_t>(((color >> 8) & 0xFF) * intensity);
    const uint8_t b = static_cast<uint8_t>((color & 0xFF) * intensity);
    const uint8_t a = (color >> 24) & 0xFF;

    // 8. Reconstruction de la couleur ARGB
    return (static_cast<uint32_t>(a) << 24)
         | (static_cast<uint32_t>(r) << 16)
         | (static_cast<uint32_t>(g) << 8)
         | static_cast<uint32_t>(b);
    
}





