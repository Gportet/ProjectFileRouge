#pragma once
#include "MathLib/MathLib_SIMD/include/Vec3_SIMD.h"

namespace ECS
{

    struct LightComponent
    {
        MathsSIMD::Vec3_SIMD direction{0.4f, 0.8f, 0.45f};
        float ambient = 0.18f;
        float specularStrength = 0.6f;
        MathsSIMD::Vec3_SIMD viewDir{0.0f, 0.0f, 1.0f};
    };
}