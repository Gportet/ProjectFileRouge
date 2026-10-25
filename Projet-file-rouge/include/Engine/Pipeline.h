#pragma once
#include <vector>
#include <cstdint>
#include "Mesh.h"
#include "Engine/ZBuffer.h"
#include "Engine/Light.h"
#include "MathLib_SIMD/include/Vec4_SIMD.h"

#include <memory>
#include "Engine/Rendering/BlinnPhongShader.h"
#include "Engine/Rendering/LambertShader.h"
#include "Engine/Rendering/PhongShader.h"
#include "Engine/Rendering/UnlitShader.h"


struct ClipVertex {
    MathsSIMD::Vec4_SIMD clipPos;
    MathsSIMD::Vec3_SIMD worldPosition;
    MathsSIMD::Vec3_SIMD normal;
    Vec2 uv;
    
    static ClipVertex Lerp(const ClipVertex& a, const ClipVertex& b, float t) {
        ClipVertex v;
        v.clipPos = MathsSIMD::Vec4_SIMD::Lerp(a.clipPos, b.clipPos, t);
        v.worldPosition = MathsSIMD::Vec3_SIMD::Lerp(a.worldPosition, b.worldPosition, t);
        v.normal  = MathsSIMD::Vec3_SIMD::Lerp(a.normal, b.normal, t);
        v.uv.u    = a.uv.u + (b.uv.u - a.uv.u) * t;
        v.uv.v    = a.uv.v + (b.uv.v - a.uv.v) * t;
        return v;
    }
};

struct ClipTriangle { 
    ClipVertex v0, v1, v2;
};


class Pipeline {
private:
    std::unique_ptr<Shader> m_Shader;
    int m_width;
    int m_height;
    std::vector<uint8_t> m_pixels;
    ZBuffer m_zBuffer;

    void PutPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    int Edge(Point a, Point b, Point p);
    bool ClipLine(int& x0, int& y0, int& x1, int& y1);

public:
    Pipeline(int width, int height);
    
    void Clear();
    void DrawLine(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void DrawTriangle(
        Point v0, Point v1, Point v2,
        float z0, float z1, float z2,
        float iw0, float iw1, float iw2,
        Vec2 uv0, Vec2 uv1, Vec2 uv2,
        MathsSIMD::Vec3_SIMD pos0,
        MathsSIMD::Vec3_SIMD pos1,
        MathsSIMD::Vec3_SIMD pos2,
        MathsSIMD::Vec3_SIMD n0,
        MathsSIMD::Vec3_SIMD n1,
        MathsSIMD::Vec3_SIMD n2,
        Texture* texture,
        const DirectionalLight& light,
        const MathsSIMD::Vec3_SIMD& cameraPosition
    );
    bool BackCulling(Point v0, Point v1, Point v2);

    const uint8_t* GetColorBuffer() const { return m_pixels.data(); }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    
    static int ClipTriangleNear(const ClipVertex& v0, const ClipVertex& v1, const ClipVertex& v2, ClipTriangle outTris[2]);
};