#include "MathLib/MathLib_SIMD/include/Matrix4x4_SIMD.h"
#include "MathLib/MathLib_SIMD/include/Vec3_SIMD.h"

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <cmath>
#include <algorithm>


#define PI 3.14159265358979323846f

const int WIDTH = 800;
const int HEIGHT = 600;

struct Point
{
    int x;
    int y;
};

static inline uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)0xFFu << 24) | ((uint32_t)r << 16)
         | ((uint32_t)g    <<  8) |  (uint32_t)b;
}

static inline uint32_t shade(int e0, int e1, int e2, int wsum,
                             uint32_t c0, uint32_t c1, uint32_t c2) {
    uint8_t r = (uint8_t)((e0 * ((c0 >> 16) & 0xFF)
                         + e1 * ((c1 >> 16) & 0xFF)
                         + e2 * ((c2 >> 16) & 0xFF)) / wsum);
    uint8_t g = (uint8_t)((e0 * ((c0 >>  8) & 0xFF)
                         + e1 * ((c1 >>  8) & 0xFF)
                         + e2 * ((c2 >>  8) & 0xFF)) / wsum);
    uint8_t b = (uint8_t)((e0 * ( c0        & 0xFF)
                         + e1 * ( c1        & 0xFF)
                         + e2 * ( c2        & 0xFF)) / wsum);
    return rgb(r, g, b);
}

void put_pixel(std::vector<uint8_t>& buffer, int x, int y,
               uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    int index = (y * WIDTH + x) * 4;
    buffer[index + 0] = r;
    buffer[index + 1] = g;
    buffer[index + 2] = b;
    buffer[index + 3] = a;
}

std::vector<uint8_t> generate_gradient(int width, int height) {
    std::vector<uint8_t> buffer(width * height * 4);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint8_t r = static_cast<uint8_t>((float)x / width * 255);
            uint8_t g = static_cast<uint8_t>((float)y / height * 255);
            uint8_t b = 128;
            put_pixel(buffer, x, y, r, g, b);
        }
    }
    return buffer;
}

void draw_line(std::vector<uint8_t>& buffer, int width, int height,
               int x0, int y0, int x1, int y1, 
               uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    int dx =  std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);         
    int sx = (x0 < x1) ? 1 : -1;     
    int sy = (y0 < y1) ? 1 : -1;     
    int err = dx + dy;
    
    while (true)
    {
        if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height) {
            put_pixel(buffer, x0, y0, r, g, b, a);
        }

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

std::vector<uint8_t> draw_MulLine(double x1, double y1, int nb_line, int radius)
{
    std::vector<uint8_t> buffer(WIDTH * HEIGHT * 4, 0);
    double x0 = static_cast<double>(WIDTH / 2);
    double y0 = static_cast<double>(HEIGHT / 2);
    
    for (int i = 0; i < nb_line; ++i)
    {
        float angle = i * (2.0f * 3.14159f / nb_line);
        
        int line_x1 = x0 + static_cast<int>(radius * std::cos(angle));
        int line_y1 = y0 + static_cast<int>(radius * std::sin(angle));

        draw_line(buffer, WIDTH, HEIGHT, line_x1, line_y1, static_cast<int>(x0), static_cast<int>(y0), 0, 0, 255);
    }
    return buffer;
}

int edge(Point v0, Point v1, Point v2)
{
    return (v1.x - v0.x) * (v2.y - v0.y) - (v1.y - v0.y) * (v2.x - v0.x);
}

std::vector<uint8_t> draw_Triangle(std::vector<uint8_t> framebuffer, Point v0, Point v1, Point v2, uint32_t c0, uint32_t c1, uint32_t c2)
{
    std::vector<uint8_t> newBuffer = framebuffer;
    int area = edge(v0, v1, v2);

    if (area == 0) return newBuffer;

    if (area < 0)
    {
        std::swap(v1, v2);
        std::swap(c1, c2);
        area = -area;
    }
    
    // Bounding box, clamped to the screen.
    int minx = std::min({v0.x, v1.x, v2.x});
    int miny = std::min({v0.y, v1.y, v2.y});
    int maxx = std::max({v0.x, v1.x, v2.x});
    int maxy = std::max({v0.y, v1.y, v2.y});

    minx = std::max(0, minx);
    miny = std::max(0, miny);
    maxx = std::min(WIDTH - 1, maxx);
    maxy = std::min(HEIGHT - 1, maxy);
    
    for (int y = miny; y <= maxy; y++) {
        for (int x = minx; x <= maxx; x++) {
            Point p = {x, y};
            int w0 = edge(v1, v2, p); // v1->v2 (poids pour v0)
            int w1 = edge(v2, v0, p); // v2->v0 (poids pour v1)
            int w2 = edge(v0, v1, p); // v0->v1 (poids pour v2)

            // Inside iff all three weights are non-negative.
            if ((w0 | w1 | w2) >= 0) {
                uint32_t col = shade(w0, w1, w2, area, c0, c1, c2);
                uint8_t r = (col >> 16) & 0xFF;
                uint8_t g = (col >> 8)  & 0xFF;
                uint8_t b =  col        & 0xFF;
                put_pixel(newBuffer, x, y, r, g, b);
            }
        }
    }
    return newBuffer;
}

void draw_RotatingTriangle(
    std::vector<uint8_t>& buffer,
    float angle,
    uint32_t c0,
    uint32_t c1,
    uint32_t c2)
{
    // Sommets du triangle centrés autour de l'origine
    constexpr float R = 200.0f;
    MathsSIMD::Vec3_SIMD base[3];

    for (int i = 0; i < 3; ++i)
    {
        const float a =
            static_cast<float>(i) / 3.0f * 2.0f * PI
            - PI / 2.0f;

        base[i] = MathsSIMD::Vec3_SIMD(
            R * std::cos(a),
            R * std::sin(a),
            0.0f
        );
    }

    // Rotation autour de l'origine, puis translation au centre
    const auto matrix =
        MathsSIMD::Matrix4x4_SIMD::Translation(
            MathsSIMD::Vec3_SIMD(
                WIDTH / 2.0f,
                HEIGHT / 2.0f,
                0.0f
            )
        )
        * MathsSIMD::Matrix4x4_SIMD::RotationZ(angle);

    // Transformation SIMD des sommets
    MathsSIMD::Vec3_SIMD p[3];

    for (int i = 0; i < 3; ++i)
    {
        p[i] = matrix.TransformPoint(base[i]);
    }

    // Conversion en coordonnées écran
    const Point v0 = {
        static_cast<int>(p[0].data.m128_f32[0]),
        static_cast<int>(p[0].data.m128_f32[1])
    };

    const Point v1 = {
        static_cast<int>(p[1].data.m128_f32[0]),
        static_cast<int>(p[1].data.m128_f32[1])
    };

    const Point v2 = {
        static_cast<int>(p[2].data.m128_f32[0]),
        static_cast<int>(p[2].data.m128_f32[1])
    };

    // Effacer la frame précédente
    std::fill(buffer.begin(), buffer.end(), 0);

    // Rasteriser le triangle
    buffer = draw_Triangle(
        buffer, v0, v1, v2,
        c0, c1, c2
    );
}

static const MathsSIMD::Vec3_SIMD CUBE_VERTS[8] =
{
    {-1.0f, -1.0f, -1.0f},
    { 1.0f, -1.0f, -1.0f},
    { 1.0f,  1.0f, -1.0f},
    {-1.0f,  1.0f, -1.0f},

    {-1.0f, -1.0f,  1.0f},
    { 1.0f, -1.0f,  1.0f},
    { 1.0f,  1.0f,  1.0f},
    {-1.0f,  1.0f,  1.0f}
};


// Chaque ligne : { sommet de départ, sommet d'arrivée, axe }
static const int CUBE_EDGES[12][3] =
{
    // Arêtes selon X
    {0, 1, 0},
    {3, 2, 0},
    {7, 6, 0},
    {4, 5, 0},

    // Arêtes selon Y
    {0, 3, 1},
    {1, 2, 1},
    {5, 6, 1},
    {4, 7, 1},

    // Arêtes selon Z
    {0, 4, 2},
    {1, 5, 2},
    {2, 6, 2},
    {3, 7, 2}
};


struct Color
{
    uint8_t r, g, b, a;
};

static const Color axis_color[3] = {
    {255,   0,   0, 255}, // Axe X : rouge
    {  0, 255,   0, 255}, // Axe Y : vert
    {  0,   0, 255, 255}  // Axe Z : bleu
};

const MathsSIMD::Vec3_SIMD eye    = {0.0f, 0.0f, 5.0f};
const MathsSIMD::Vec3_SIMD target = {0.0f, 0.0f, 0.0f};
const MathsSIMD::Vec3_SIMD up     = {0.0f, 1.0f, 0.0f};

int main() {
    if (!glfwInit()) {
        std::cerr << "Erreur GLFW" << std::endl;
        return -1;
    }

    
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Degrade RGB", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erreur GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    std::vector<uint8_t> pixels(WIDTH * HEIGHT * 4, 0);

    // Sommets du triangle de base, centrés autour de l'origine
    MathsSIMD::Vec3_SIMD base[3];
    const float R = 200.0f;

    for (int i = 0; i < 3; i++) {
        float a = (float)i / 3.0f * 2.0f * PI - PI / 2.0f;

        base[i] = MathsSIMD::Vec3_SIMD(
            R * cosf(a),
            R * sinf(a),
            0.0f
        );
    }

    // Couleurs des sommets
    uint32_t c0 = rgb(255, 0, 0);
    uint32_t c1 = rgb(0, 255, 0);
    uint32_t c2 = rgb(0, 0, 255);

    // Création de la texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        WIDTH, HEIGHT, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );

    // Création du framebuffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
        GL_READ_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        textureID,
        0
    );

    double last = glfwGetTime();
    double fps_accum = 0.0;
    int fps_count = 0;

    char title[256];

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        int display_w = 0;
        int display_h = 0;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        // Dessiner le triangle avec un angle qui évolue dans le temps
        //const float angle = static_cast<float>(glfwGetTime());
        //draw_RotatingTriangle(pixels, angle, c0, c1, c2);

        const float t = glfwGetTime();
        MathsSIMD::Matrix4x4_SIMD model = MathsSIMD::Matrix4x4_SIMD::RotationY(t * 0.8f) * MathsSIMD::Matrix4x4_SIMD::RotationX(t * 0.5f);
        
        MathsSIMD::Matrix4x4_SIMD MVP =  MathsSIMD::Matrix4x4_SIMD::MVP_Matrix(model, eye, target, up,90, WIDTH, HEIGHT);
        
        int screen_x[8], screen_y[8];
        bool visible[8];

        for (int i = 0; i < 8; ++i)
        {
            const auto& vertex = CUBE_VERTS[i];

            MathsSIMD::Vec4_SIMD point(_mm_setr_ps(
                MathsSIMD::Vec3_SIMD::GetX(vertex.data),
                MathsSIMD::Vec3_SIMD::GetY(vertex.data),
                MathsSIMD::Vec3_SIMD::GetZ(vertex.data),
                1.0f
            ));

            const MathsSIMD::Vec4_SIMD clip = MVP * point;

            const float w = MathsSIMD::Vec4_SIMD::GetW(clip.data);

            if (w <= 0.0001f || !std::isfinite(w))
            {
                visible[i] = false;
                continue;
            }

            const float ndc_x =
                MathsSIMD::Vec3_SIMD::GetX(clip.data) / w;

            const float ndc_y =
                MathsSIMD::Vec3_SIMD::GetY(clip.data) / w;

            if (!std::isfinite(ndc_x) || !std::isfinite(ndc_y))
            {
                visible[i] = false;
                continue;
            }

            screen_x[i] = static_cast<int>(
                (ndc_x + 1.0f) * 0.5f * WIDTH
            );

            screen_y[i] = static_cast<int>(
                (1.0f - ndc_y) * 0.5f * HEIGHT
            );

            visible[i] = true;
        }
        
        
        for (int i = 0; i < 12; ++i)
        {
            const int v0   = CUBE_EDGES[i][0];
            const int v1   = CUBE_EDGES[i][1];
            const int axis = CUBE_EDGES[i][2];

            if (!visible[v0] || !visible[v1])
                continue;

            const Color& color = axis_color[axis];

            draw_line(
                pixels,
                WIDTH,
                HEIGHT,
                screen_x[v0],
                screen_y[v0],
                screen_x[v1],
                screen_y[v1],
                color.r,
                color.g,
                color.b,
                color.a
            );
        }
        
        
        // Envoyer les pixels à la texture OpenGL
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexSubImage2D(
            GL_TEXTURE_2D, 0, 0, 0,
            WIDTH, HEIGHT,
            GL_RGBA, GL_UNSIGNED_BYTE,
            pixels.data()
        );

        // Afficher la texture
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glBlitFramebuffer(
            0, 0, WIDTH, HEIGHT,
            0, 0, display_w, display_h,
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );

        // Conserver ici ton calcul des FPS existant.

        glfwSwapBuffers(window);
    }

    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &textureID);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
