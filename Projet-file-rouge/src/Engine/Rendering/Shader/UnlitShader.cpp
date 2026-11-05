#include "Engine/Rendering/Shader/UnlitShader.h"

uint32_t UnlitShader::ShaderFragment(const MathsSIMD::Vec3_SIMD& cameraPosition, const MathsSIMD::Vec3_SIMD& position, const MathsSIMD::Vec3_SIMD& normals, const DirectionalLight& light, const Vec2& uv, const Material& material) const
{
    // Récupère la couleur de la texture aux coordonnées UV
    const uint32_t color = material.getTexture()->SampleNearest(uv.u, uv.v);

    // Retourne directement la couleur du pixel
    return color;
}
