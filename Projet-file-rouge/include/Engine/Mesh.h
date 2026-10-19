#pragma once
#include <vector>
#include "MathLib/MathLib_SIMD/include/Vec3_SIMD.h"

struct Point {
    int x;
    int y;
};

struct Face {
    int v0, v1, v2;
    int vt0, vt1, vt2;
};

struct Vec2 {
    float u, v;
};

struct Mesh {
    std::vector<MathsSIMD::Vec3_SIMD> vertices;
    std::vector<Face> faces;
    std::vector<Vec2> uvs;
};

