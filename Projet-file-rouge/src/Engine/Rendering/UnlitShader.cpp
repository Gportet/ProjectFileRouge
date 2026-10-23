#include "Engine/Rendering/UnlitShader.h"

uint32_t UnlitShader::ShaderFragment(const MathsSIMD::Vec3_SIMD& cameraPosition, const MathsSIMD::Vec3_SIMD& position, const MathsSIMD::Vec3_SIMD& normals, const DirectionalLight& light, const Vec2& uv, const Texture& texture) const
{
    // Récupère la couleur de la texture aux coordonnées UV
    const uint32_t color = texture.SampleNearest(uv.u, uv.v);

    // Retourne directement la couleur du pixel
    return color;
}
