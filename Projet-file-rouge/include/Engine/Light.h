#ifndef LIGHT_H
#define LIGHT_H

#include "MathLib/MathLib_SIMD/include/Vec3_SIMD.h"
#include <algorithm>

struct DirectionalLight {
    MathsSIMD::Vec3_SIMD direction{ 0.4f, 0.8f, 0.45f };
    float ambient = 0.18f;//eviter que ce soit trop sombre
    
    [[nodiscard]] float CalculateBrightness(const MathsSIMD::Vec3_SIMD& worldNormal) const {
        MathsSIMD::Vec3_SIMD l = direction.Normalize();


        float ndotl = worldNormal.Dot(l);
        if (ndotl < 0.0f) ndotl = 0.0f;

        return ambient + (1.0f - ambient) * ndotl;
    }
};

#endif 