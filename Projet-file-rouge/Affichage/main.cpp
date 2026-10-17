#include "MathLib/MathLib_SIMD/include/Matrix4x4_SIMD.h"
#include "MathLib/MathLib_SIMD/include/Vec3_SIMD.h"
#include "Rendering/ZBuffer.h"

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

#define PI 3.14159265358979323846f

const int WIDTH = 800;
const int HEIGHT = 600;

struct Point
{
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

Mesh load_obj(const std::string& filename) {
    Mesh mesh;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Impossible d'ouvrir le fichier : " << filename << std::endl;
        return mesh;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            float x, y, z;
            if (ss >> x >> y >> z) {
                mesh.vertices.push_back(MathsSIMD::Vec3_SIMD(x, y, z));
            }
        }
        else if (type == "f") {
            int v[3] = {0, 0, 0};
            bool valid = true;
            for (int i = 0; i < 3; ++i) {
            std::string token;
                if (ss >> token) {
                    std::stringstream token_stream(token);
                    if (!(token_stream >> v[i])) valid = false;
                } else {
                    valid = false;
                }
            }
            if (valid && v[0] > 0 && v[1] > 0 && v[2] > 0) {
                mesh.faces.push_back(Face{ v[0] - 1, v[1] - 1, v[2] - 1 });
            }
        }
    }
    return mesh;
}

void put_pixel(std::vector<uint8_t>& buffer, int x, int y,
               uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    int index = (y * WIDTH + x) * 4;
    buffer[index + 0] = r;
    buffer[index + 1] = g;
    buffer[index + 2] = b;
    buffer[index + 3] = a;
}

int edge(Point a, Point b, Point p)
{
    return (p.x - a.x) * (b.y - a.y)
         - (p.y - a.y) * (b.x - a.x);
}

void draw_Triangle(
    std::vector<uint8_t>& buffer,
    ZBuffer& zBuffer,
    Point v0, Point v1, Point v2,
    float z0, float z1, float z2)
{
    int area = edge(v0, v1, v2);

    if (area == 0)
        return;

    if (area < 0)
    {
        std::swap(v1, v2);
        std::swap(z1, z2);
        area = -area;
    }

    const int minX = std::max(0, std::min({v0.x, v1.x, v2.x}));
    const int minY = std::max(0, std::min({v0.y, v1.y, v2.y}));
    const int maxX = std::min(WIDTH - 1, std::max({v0.x, v1.x, v2.x}));
    const int maxY = std::min(HEIGHT - 1, std::max({v0.y, v1.y, v2.y}));

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            Point p = {x, y};

            int w0 = edge(v1, v2, p);
            int w1 = edge(v2, v0, p);
            int w2 = edge(v0, v1, p);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
            {
                float lambda0 = static_cast<float>(w0) / area;
                float lambda1 = static_cast<float>(w1) / area;
                float lambda2 = static_cast<float>(w2) / area;

                float depth =
                    lambda0 * z0 +
                    lambda1 * z1 +
                    lambda2 * z2;

                if (zBuffer.TestAndApply(x, y, depth))
                {
                    put_pixel(buffer, x, y, 255, 255, 255);
                }
            }
        }
    }
}


void clear_pixels(std::vector<uint8_t>& buffer) {
    for (size_t i = 0; i < buffer.size(); i += 4) {
        buffer[i + 0] = 0;   // R
        buffer[i + 1] = 0;   // G
        buffer[i + 2] = 0;   // B
        buffer[i + 3] = 255; // Alpha
    }
}

bool clip_line(int& x0, int& y0, int& x1, int& y1, int width, int height) {
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
        edge_check( dx, static_cast<float>(width - 1 - x0)) &&
        edge_check(-dy, static_cast<float>(y0)) &&
        edge_check( dy, static_cast<float>(height - 1 - y0))) {
        
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

void draw_line(std::vector<uint8_t>& buffer, int width, int height,
               int x0, int y0, int x1, int y1, 
               uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    if (!clip_line(x0, y0, x1, y1, width, height)) {
        return;
    }

    int dx =  std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);         
    int sx = (x0 < x1) ? 1 : -1;     
    int sy = (y0 < y1) ? 1 : -1;     
    int err = dx + dy;
    
    while (true)
    {
        put_pixel(buffer, x0, y0, r, g, b, a);

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

const MathsSIMD::Vec3_SIMD eye    = {0.0f, 0.0f, 3.0f};
const MathsSIMD::Vec3_SIMD target = {0.0f, 0.0f, 0.0f};
const MathsSIMD::Vec3_SIMD up     = {0.0f, 1.0f, 0.0f};

bool BackCulling(Point v0, Point v1, Point v2)
{
    return edge(v0, v1, v2) > 0;
}


int main() {
    if (!glfwInit()) {
        std::cerr << "Erreur GLFW" << std::endl;
        return -1;
    }

    Mesh spot = load_obj("C:/Users/jcrap/Documents/GitHub/ProjectFileRouge/Projet-file-rouge/Affichage/spot.obj");
    std::cout << "Sommets chargés : " << spot.vertices.size() << std::endl;
    std::cout << "Faces chargées : " << spot.faces.size() << std::endl;

    if (spot.vertices.empty() || spot.faces.empty()) {
        std::cerr << "Erreur : Maillage vide ou non chargé." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Affichage Spot OBJ", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erreur GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    std::vector<uint8_t> pixels(WIDTH * HEIGHT * 4, 0);
    ZBuffer Zbuffer (WIDTH, HEIGHT);
    

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

    std::vector<int> screen_x(spot.vertices.size());
    std::vector<int> screen_y(spot.vertices.size());
    std::vector<float> screen_z(spot.vertices.size());
    std::vector<bool> visible(spot.vertices.size());

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

        const float t = static_cast<float>(glfwGetTime());
        MathsSIMD::Matrix4x4_SIMD model = MathsSIMD::Matrix4x4_SIMD::RotationY(t * 0.5f);
        MathsSIMD::Matrix4x4_SIMD MVP = MathsSIMD::Matrix4x4_SIMD::MVP_Matrix(model, eye, target, up, 90, WIDTH, HEIGHT);

        for (size_t i = 0; i < spot.vertices.size(); ++i)
        {
            const auto& vertex = spot.vertices[i];

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

            const float ndc_x = MathsSIMD::Vec3_SIMD::GetX(clip.data) / w;
            const float ndc_y = MathsSIMD::Vec3_SIMD::GetY(clip.data) / w;
            const float ndc_z = MathsSIMD::Vec3_SIMD::GetZ(clip.data) / w;

            if (!std::isfinite(ndc_x) || !std::isfinite(ndc_y) || !std::isfinite(ndc_z))
            {
                visible[i] = false;
                continue;
            }

            screen_x[i] = static_cast<int>((ndc_x + 1.0f) * 0.5f * WIDTH);
            screen_y[i] = static_cast<int>((1.0f - ndc_y) * 0.5f * HEIGHT);
            screen_z[i] = ndc_z;
            visible[i] = true;
        }

        clear_pixels(pixels);
        Zbuffer.Clear();

        const int max_vert_idx = static_cast<int>(spot.vertices.size());
        
        int totalTriangles = 0;
        int displayedTriangles = 0;
        int culledTriangles = 0;
        
        for (const auto& face : spot.faces)
        {
            int v0 = face.v0;
            int v1 = face.v1;
            int v2 = face.v2;

            if (v0 < 0 || v0 >= max_vert_idx ||
                v1 < 0 || v1 >= max_vert_idx ||
                v2 < 0 || v2 >= max_vert_idx)
            {
                continue;
            }
            
            if (!visible[v0] || !visible[v1] || !visible[v2])
                continue;

            // Backface Culling
            if (!BackCulling(
                {screen_x[v0], screen_y[v0]},
                {screen_x[v1], screen_y[v1]},
                {screen_x[v2], screen_y[v2]}
                
            ))
            {
                continue;
            }
            
            draw_Triangle(
                pixels,
                Zbuffer,
                {screen_x[v0], screen_y[v0]},
                {screen_x[v1], screen_y[v1]},
                {screen_x[v2], screen_y[v2]},
                screen_z[v0],
                screen_z[v1],
                screen_z[v2]
            );
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, display_w, display_h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glfwSwapBuffers(window);
        // ---- Update the FPS counter ----
        double now = glfwGetTime();
        double dt  = now - last;
        last       = now;

        fps_accum += dt;
        fps_count++;

        if (fps_accum >= 1.0) {
            double fps = (double)fps_count / fps_accum;
            double ms  = 1000.0 * fps_accum / (double)fps_count;

            snprintf(title, sizeof(title),
                     "Chapter 02 — Lines | %.1f FPS (%.2f ms/frame)",
                     fps, ms);

            glfwSetWindowTitle(window, title);

            fps_accum = 0.0;
            fps_count = 0;
        }
    }
    
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &textureID);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}