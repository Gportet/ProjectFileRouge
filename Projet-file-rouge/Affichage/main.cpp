#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

void put_pixel(std::vector<uint8_t>& buffer, uint32_t x, uint32_t y, uint32_t width, 
               uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    uint32_t index = (y * width + x) * 4;
    buffer[index + 0] = r;
    buffer[index + 1] = g;
    buffer[index + 2] = b;
    buffer[index + 3] = a;
}

std::vector<uint8_t> generate_gradient(uint32_t width, uint32_t height) {
    std::vector<uint8_t> buffer(width * height * 4);
    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            uint8_t r = static_cast<uint8_t>((float)x / width * 255);
            uint8_t g = static_cast<uint8_t>((float)y / height * 255);
            uint8_t b = 128;
            put_pixel(buffer, x, y, width, r, g, b);
        }
    }
    return buffer;
}

void draw_line(std::vector<uint8_t>& buffer, uint32_t width, uint32_t height,
               int x0, int y0, int x1, int y1, 
               uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
{
    int dx =  std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);         
    int sx = (x0 < x1) ? 1 : -1;     
    int sy = (y0 < y1) ? 1 : -1;     
    int err = dx + dy;
    
    while (true)
    {
        if (x0 >= 0 && x0 < static_cast<int>(width) && y0 >= 0 && y0 < static_cast<int>(height)) {
            put_pixel(buffer, static_cast<uint32_t>(x0), static_cast<uint32_t>(y0), width, r, g, b, a);
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


std::vector<uint8_t> draw_MulLine(double x1, double y1,int nb_line, int radius)
{
    std::vector<uint8_t> buffer(WIDTH * HEIGHT * 4, 0);
    //origin
    double x0 = static_cast<double>(WIDTH/2);
    double y0 = static_cast<double>(HEIGHT/2);
    
    for (int i = 0; i < nb_line; ++i)
    {
        float angle = i * (2.0f * 3.14159f / nb_line);
        
        int x1 = x0 + static_cast<int>(radius * std::cos(angle));
        int y1 = y0 + static_cast<int>(radius * std::sin(angle));


        draw_line(buffer, WIDTH, HEIGHT, x1, y1, x0, y0,0,0,255);
    }
    return buffer;
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

    std::vector<uint8_t> pixels = draw_MulLine(500,500,16, 100);
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
                     "Chapter 02 — Lines | %.1f FPS (%.2f ms/frame)",
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