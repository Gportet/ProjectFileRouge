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
    
    std::vector<MathsSIMD::Vec3_SIMD> faceNormals;
    std::vector<MathsSIMD::Vec3_SIMD> vertexNormals;
    
    void ComputeFaceNormals();
    void ComputeVertexNormals();
};


class Texture
{
private:
    std::vector<uint32_t> pixels;
    int width = 0;
    int height = 0;

public:
    static bool texture_load(const char* path, Texture* texture);
    static void texture_free(Texture* texture);
    static uint32_t texture_sample(const Texture* texture, float u, float v);

    int getWidth() const { return width; }
    int getHeight() const { return height; }
};
