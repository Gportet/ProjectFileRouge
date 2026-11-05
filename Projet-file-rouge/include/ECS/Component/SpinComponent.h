#pragma once
#include "MathLib/MathLib_SIMD/include/Vec3_SIMD.h"

namespace ECS
{
    struct SpinComponent
    {
        MathsSIMD::Vec3 radiansPerSecond{0.0f, 0.5f, 0.0f};   // 0.5 rad/s autour de Y : la vitesse de l'ancien main
    };
}
