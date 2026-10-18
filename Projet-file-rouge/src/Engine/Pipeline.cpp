#include "Engine/Pipeline.h"
#include <algorithm>
#include <cmath>

Pipeline::Pipeline(int width, int height) 
    : m_width(width), m_height(height), 
      m_pixels(width * height * 4, 0), 
      m_zBuffer(width, height) {}

void Pipeline::Clear() {
    for (size_t i = 0; i < m_pixels.size(); i += 4) {
        m_pixels[i + 0] = 0;   
        m_pixels[i + 1] = 0;   
        m_pixels[i + 2] = 0;   
        m_pixels[i + 3] = 255; 
    }
    m_zBuffer.Clear();
}

void Pipeline::PutPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) return;
    int index = (y * m_width + x) * 4;
    m_pixels[index + 0] = r;
    m_pixels[index + 1] = g;
    m_pixels[index + 2] = b;
    m_pixels[index + 3] = a;
}

int Pipeline::Edge(Point a, Point b, Point p) {
    return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}

bool Pipeline::BackCulling(Point v0, Point v1, Point v2) {
    return Edge(v0, v1, v2) > 0;
}

void Pipeline::DrawTriangle(Point v0, Point v1, Point v2, float z0, float z1, float z2) {
    int area = Edge(v0, v1, v2);

    if (area == 0)
        return;

    if (area < 0) {
        std::swap(v1, v2);
        std::swap(z1, z2);
        area = -area;
    }

    const int minX = std::max(0, std::min({v0.x, v1.x, v2.x}));
    const int minY = std::max(0, std::min({v0.y, v1.y, v2.y}));
    const int maxX = std::min(m_width - 1, std::max({v0.x, v1.x, v2.x}));
    const int maxY = std::min(m_height - 1, std::max({v0.y, v1.y, v2.y}));

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            Point p = {x, y};

            int w0 = Edge(v1, v2, p);
            int w1 = Edge(v2, v0, p);
            int w2 = Edge(v0, v1, p);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                float lambda0 = static_cast<float>(w0) / area;
                float lambda1 = static_cast<float>(w1) / area;
                float lambda2 = static_cast<float>(w2) / area;

                float depth = lambda0 * z0 + lambda1 * z1 + lambda2 * z2;

                if (m_zBuffer.TestAndApply(x, y, depth)) {
                    PutPixel(x, y, 255, 255, 255);
                }
            }
        }
    }
}

bool Pipeline::ClipLine(int& x0, int& y0, int& x1, int& y1) {
    float t0 = 0.0f, t1 = 1.0f;
    float dx = static_cast<float>(x1 - x0);
    float dy = static_cast<float>(y1 - y0);

    auto edge_check = [&](float p, float q) {
        if (p == 0.0f) return q >= 0.0f;
        float r = q / p;
        if (p < 0.0f) {
            if (r > t1) return false;
            if (r > t0) t0 = r;
        } else {
            if (r < t0) return false;
            if (r < t1) t1 = r;
        }
        return true;
    };

    if (edge_check(-dx, static_cast<float>(x0)) &&
        edge_check( dx, static_cast<float>(m_width - 1 - x0)) &&
        edge_check(-dy, static_cast<float>(y0)) &&
        edge_check( dy, static_cast<float>(m_height - 1 - y0))) {
        
        if (t1 < 1.0f) {
            x1 = x0 + static_cast<int>(t1 * dx);
            y1 = y0 + static_cast<int>(t1 * dy);
        }
        if (t0 > 0.0f) {
            x0 = x0 + static_cast<int>(t0 * dx);
            y0 = y0 + static_cast<int>(t0 * dy);
        }
        return true;
    }
    return false;
}

void Pipeline::DrawLine(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (!ClipLine(x0, y0, x1, y1)) {
        return;
    }

    int dx =  std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);         
    int sx = (x0 < x1) ? 1 : -1;     
    int sy = (y0 < y1) ? 1 : -1;     
    int err = dx + dy;

    while (true) {
        PutPixel(x0, y0, r, g, b, a);

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;

        if (e2 >= dy) { 
            err += dy; 
            x0 += sx; 
        }
        if (e2 <= dx) { 
            err += dx; 
            y0 += sy; 
        }
    }
}