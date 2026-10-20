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
    return Edge(v0, v1, v2) < 0;
}

void Pipeline::DrawTriangle(Point v0, Point v1, Point v2, 
                           float z0, float z1, float z2, 
                           float iw0, float iw1, float iw2, 
                           Vec2 uv0, Vec2 uv1, Vec2 uv2, 
                           Texture *texture, 
                           float brightness) {
    int area = Edge(v0, v1, v2);

    if (area == 0)
        return;

    if (area < 0) {
        std::swap(v1, v2);
        std::swap(z1, z2);
        std::swap(iw1, iw2);
        std::swap(uv1, uv2);
        area = -area;
    }

    const int minX = std::max(0, std::min({v0.x, v1.x, v2.x}));
    const int minY = std::max(0, std::min({v0.y, v1.y, v2.y}));
    const int maxX = std::min(m_width - 1, std::max({v0.x, v1.x, v2.x}));
    const int maxY = std::min(m_height - 1, std::max({v0.y, v1.y, v2.y}));

    float inv_area = 1.0f / (float)area;
    
    float u0w = uv0.u * iw0, v0w = uv0.v * iw0;
    float u1w = uv1.u * iw1, v1w = uv1.v * iw1;
    float u2w = uv2.u * iw2, v2w = uv2.v * iw2;
    
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            Point p = {x, y};

            int w0 = Edge(v1, v2, p);
            int w1 = Edge(v2, v0, p);
            int w2 = Edge(v0, v1, p);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                float lambda0 = static_cast<float>(w0) * inv_area;
                float lambda1 = static_cast<float>(w1) * inv_area;
                float lambda2 = static_cast<float>(w2) * inv_area;

                float depth = lambda0 * z0 + lambda1 * z1 + lambda2 * z2;

                float uw = lambda0 * u0w + lambda1 * u1w + lambda2 * u2w;
                float vw = lambda0 * v0w + lambda1 * v1w + lambda2 * v2w;
                float iw = lambda0 * iw0 + lambda1 * iw1 + lambda2 * iw2;
                if (!std::isfinite(iw) || iw <= 0.0f)
                    continue;

                float u = uw / iw;
                float v = vw / iw;
                
                if (m_zBuffer.TestAndApply(x, y, depth)) {
                    uint32_t color = Texture::texture_sample(texture, u, v);
                    
                    float l = std::clamp(brightness, 0.0f, 1.0f);
                    uint8_t r = static_cast<uint8_t>(((color >> 16) & 0xFF) * l);
                    uint8_t g = static_cast<uint8_t>(((color >> 8) & 0xFF) * l);
                    uint8_t b = static_cast<uint8_t>((color & 0xFF) * l);

                    PutPixel(x, y, r, g, b);
                }
            }
        }
    }
}



void Pipeline::DrawTrianglePhong(Point v0, Point v1, Point v2, 
                               float z0, float z1, float z2, 
                               float iw0, float iw1, float iw2, 
                               Vec2 uv0, Vec2 uv1, Vec2 uv2, 
                               MathsSIMD::Vec3_SIMD n0, 
                               MathsSIMD::Vec3_SIMD n1, 
                               MathsSIMD::Vec3_SIMD n2,
                               Texture *texture, 
                               const DirectionalLight& light,
                               const MathsSIMD::Vec3_SIMD& halfVector) {
    int area = Edge(v0, v1, v2);
    if (area == 0) return;

    if (area < 0) {
        std::swap(v1, v2);
        std::swap(z1, z2);
        std::swap(iw1, iw2);
        std::swap(uv1, uv2);
        std::swap(n1, n2);
        area = -area;
    }

    const int minX = std::max(0, std::min({v0.x, v1.x, v2.x}));
    const int minY = std::max(0, std::min({v0.y, v1.y, v2.y}));
    const int maxX = std::min(m_width - 1, std::max({v0.x, v1.x, v2.x}));
    const int maxY = std::min(m_height - 1, std::max({v0.y, v1.y, v2.y}));

    float inv_area = 1.0f / static_cast<float>(area);

    float u0w = uv0.u * iw0, v0w = uv0.v * iw0;
    float u1w = uv1.u * iw1, v1w = uv1.v * iw1;
    float u2w = uv2.u * iw2, v2w = uv2.v * iw2;

    // Pré-division des normales par w
    MathsSIMD::Vec3_SIMD n0w = n0 * iw0;
    MathsSIMD::Vec3_SIMD n1w = n1 * iw1;
    MathsSIMD::Vec3_SIMD n2w = n2 * iw2;

    MathsSIMD::Vec3_SIMD L = light.direction.Normalize();

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            Point p = {x, y};

            int w0 = Edge(v1, v2, p);
            int w1 = Edge(v2, v0, p);
            int w2 = Edge(v0, v1, p);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                float lambda0 = static_cast<float>(w0) * inv_area;
                float lambda1 = static_cast<float>(w1) * inv_area;
                float lambda2 = static_cast<float>(w2) * inv_area;

                float depth = lambda0 * z0 + lambda1 * z1 + lambda2 * z2;

                if (m_zBuffer.TestAndApply(x, y, depth)) {
                    float iw = lambda0 * iw0 + lambda1 * iw1 + lambda2 * iw2;
                    if (!std::isfinite(iw) || iw <= 0.0f) continue;

                    float inv_iw = 1.0f / iw;
                    
                    float u = (lambda0 * u0w + lambda1 * u1w + lambda2 * u2w) * inv_iw;
                    float v = (lambda0 * v0w + lambda1 * v1w + lambda2 * v2w) * inv_iw;
                    
                    MathsSIMD::Vec3_SIMD interpolatedN = (n0w * lambda0 + n1w * lambda1 + n2w * lambda2) * inv_iw;
                    MathsSIMD::Vec3_SIMD N = interpolatedN.Normalize();
                    
                    float ndotl = std::max(0.0f, N.Dot(L));
                    float diffuse = light.ambient + (1.0f - light.ambient) * ndotl;

                    // Blinn-Phong
                    float ndoth = std::max(0.0f, N.Dot(halfVector));
                    float spec = pow32(ndoth) * light.specularStrength;
                    
                    uint32_t texel = Texture::texture_sample(texture, u, v);
                    float texR = static_cast<float>((texel >> 16) & 0xFF);
                    float texG = static_cast<float>((texel >> 8) & 0xFF);
                    float texB = static_cast<float>(texel & 0xFF);

                    uint8_t r = static_cast<uint8_t>(std::min(255.0f, texR * diffuse + 255.0f * spec));
                    uint8_t g = static_cast<uint8_t>(std::min(255.0f, texG * diffuse + 255.0f * spec));
                    uint8_t b = static_cast<uint8_t>(std::min(255.0f, texB * diffuse + 255.0f * spec));

                    PutPixel(x, y, r, g, b);
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

int Pipeline::ClipTriangleNear(const ClipVertex& v0, const ClipVertex& v1, const ClipVertex& v2, ClipTriangle outTris[2]) {
    ClipVertex inVerts[3] = { v0, v1, v2 };
    ClipVertex outVerts[4];
    int outCount = 0;

    auto GetNearDistance = [](const ClipVertex& v) -> float {
        alignas(16) float raw[4];
        _mm_store_ps(raw, v.clipPos.data);
        return raw[2] + raw[3]; 
    };
    
    for (int i = 0; i < 3; ++i) {
        const ClipVertex& curr = inVerts[i];
        const ClipVertex& next = inVerts[(i + 1) % 3];

        float dCurr = GetNearDistance(curr);
        float dNext = GetNearDistance(next);

        bool currIn = (dCurr >= 0.0f);
        bool nextIn = (dNext >= 0.0f);

        if (currIn && nextIn) {
            outVerts[outCount++] = next;
        } 
        else if (currIn && !nextIn) {
            float t = dCurr / (dCurr - dNext);
            outVerts[outCount++] = ClipVertex::Lerp(curr, next, t);
        } 
        else if (!currIn && nextIn) {
            float t = dCurr / (dCurr - dNext);
            outVerts[outCount++] = ClipVertex::Lerp(curr, next, t);
            outVerts[outCount++] = next;
        }
    }

    if (outCount < 3) return 0; 

    outTris[0] = { outVerts[0], outVerts[1], outVerts[2] };
    
    if (outCount == 4) {
        outTris[1] = { outVerts[0], outVerts[2], outVerts[3] };
        return 2;
    }

    return 1;
}