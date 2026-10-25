#include "Engine/Rendering/BlinnPhongShader.h"


uint32_t BlinnPhongShader::ShaderFragment(const MathsSIMD::Vec3_SIMD& cameraPosition,const MathsSIMD::Vec3_SIMD& position,const MathsSIMD::Vec3_SIMD& normals,const DirectionalLight& light,const Vec2& UV,const Texture& texture) const
{
    // 1. Normalisation des vecteurs
    const MathsSIMD::Vec3_SIMD N = normals.Normalize();
    const MathsSIMD::Vec3_SIMD L = light.direction.Normalize();

    const MathsSIMD::Vec3_SIMD V =
        (cameraPosition - position).Normalize();

    const MathsSIMD::Vec3_SIMD H = (L + V).Normalize();

    // 2. Calcul de la composante diffuse
    const float diffuse = std::max(0.0f, N.Dot(L));

    // 3. Calcul de la composante spéculaire
    constexpr float shininess = 32.0f;

    const float specular = light.specularStrength *
        std::pow(std::max(0.0f, N.Dot(H)), shininess);

    // 4. Combinaison des trois composantes lumineuses
    const float intensity = std::clamp(
        light.ambient + diffuse + specular,
        0.0f,
        1.0f
    );

    // 5. Récupération de la couleur de la texture
    const uint32_t color = texture.SampleNearest(UV.u, UV.v);

    // 6. Application de l'intensité sur les composantes RGB
    const uint8_t r = static_cast<uint8_t>(((color >> 16) & 0xFF) * intensity);
    const uint8_t g = static_cast<uint8_t>(((color >> 8) & 0xFF) * intensity);
    const uint8_t b = static_cast<uint8_t>((color & 0xFF) * intensity);
    const uint8_t a = (color >> 24) & 0xFF;

    // 7. Reconstruction de la couleur ARGB
    return (static_cast<uint32_t>(a) << 24)
         | (static_cast<uint32_t>(r) << 16)
         | (static_cast<uint32_t>(g) << 8)
         | static_cast<uint32_t>(b);
}

