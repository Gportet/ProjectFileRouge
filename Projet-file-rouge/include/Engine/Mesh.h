#pragma once
#include <vector>
#include "MathLib/MathLib_SIMD/include/Vec3_SIMD.h"
#include "Engine/Ressources/Texture.h"

const int WIDTH = 800;
const int HEIGHT = 800;

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
    
    std::vector<MathsSIMD::Vec3_SIMD> faceNormals;
    std::vector<MathsSIMD::Vec3_SIMD> vertexNormals;
    
    void ComputeFaceNormals();
    void ComputeVertexNormals();
};


