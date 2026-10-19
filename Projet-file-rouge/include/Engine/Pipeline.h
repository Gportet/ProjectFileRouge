#pragma once
#include <vector>
#include <cstdint>
#include "Mesh.h"
#include "Engine/ZBuffer.h"

class Pipeline {
private:
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
    void DrawTriangle(Point v0, Point v1, Point v2, float z0, float z1, float z2, float iw0, float iw1, float iw2, Vec2 uv0, Vec2 uv1, Vec2 uv2, Texture *texture );
    bool BackCulling(Point v0, Point v1, Point v2);

    const uint8_t* GetColorBuffer() const { return m_pixels.data(); }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
};