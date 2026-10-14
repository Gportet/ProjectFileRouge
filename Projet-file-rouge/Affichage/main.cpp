#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

void put_pixel(std::vector<uint8_t>& buffer, uint32_t x, uint32_t y, uint32_t width, 
               uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0) {
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

    std::vector<uint8_t> pixels = generate_gradient(WIDTH, HEIGHT);

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

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        int display_w = 0;
        int display_h = 0;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, display_w, display_h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glfwSwapBuffers(window);
    }

    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &textureID);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}