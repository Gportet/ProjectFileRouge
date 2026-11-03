#pragma once

#include "Vec3_SIMD.h"

extern "C" float DotVec3ASM(const float* a, const float* b);

namespace MathsSIMD
{
    inline float DotASM(const Vec3_SIMD& a, const Vec3_SIMD& b)
    {
        return DotVec3ASM(reinterpret_cast<const float*>(&a.data),reinterpret_cast<const float*>(&b.data));
    }
}