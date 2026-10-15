#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <cmath>
#include <algorithm>

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

    std::vector<uint8_t> buffer(WIDTH * HEIGHT * 4, 0);
    Point v0{400, 150}; 
    Point v1{200, 450};
    Point v2{600, 450}; 
    
    uint32_t c0 = rgb(255, 0, 0); // Rouge 
    uint32_t c1 = rgb(0, 255, 0); // Vert 
    uint32_t c2 = rgb(0, 0, 255); // Bleu

    std::vector<uint8_t> pixels = draw_Triangle(buffer, v0, v1, v2, c0, c1, c2);
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

    double last = glfwGetTime();
    double fps_accum = 0.0;
    int fps_count = 0;

    char title[256]; 
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        int display_w = 0;
        int display_h = 0;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, display_w, display_h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

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
                     "Chapter 03 — Triangles | %.1f FPS (%.2f ms/frame)",
                     fps, ms);

            glfwSetWindowTitle(window, title);

            fps_accum = 0.0;
            fps_count = 0;
        }
        
        glfwSwapBuffers(window);
    }

    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &textureID);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}