#pragma once
#include <vector>
#include "MathLib/MathLib_SIMD/include/Vec3_SIMD.h"

struct Point {
    int x;
    int y;
};

struct Face {
    int v0, v1, v2;
};

struct Mesh {
    std::vector<MathsSIMD::Vec3_SIMD> vertices;
    std::vector<Face> faces;
};