#ifndef LIGHT_H
#define LIGHT_H

#include "MathLib/MathLib_SIMD/include/Vec3_SIMD.h"
#include <algorithm>

inline float pow32(float x) {
    float x2 = x * x;
    float x4 = x2 * x2;
    float x8 = x4 * x4;
    float x16 = x8 * x8;
    return x16 * x16;
}

struct DirectionalLight {
    MathsSIMD::Vec3_SIMD direction{ 0.4f, 0.8f, 0.45f };
    float ambient = 0.18f;//eviter que ce soit trop sombre
    float specularStrength = 0.6f;
    
    MathsSIMD::Vec3_SIMD viewDir{ 0.0f, 0.0f, 1.0f };
    
    MathsSIMD::Vec3_SIMD GetHalfVector() const {
        MathsSIMD::Vec3_SIMD l = direction.Normalize();
        MathsSIMD::Vec3_SIMD v = viewDir.Normalize();
        return (l + v).Normalize();
    }
};

#endif 