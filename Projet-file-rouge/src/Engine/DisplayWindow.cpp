#include "Engine/DisplayWindow.h"
#include <iostream>

DisplayWindow::DisplayWindow(int width, int height, const std::string& title)
    : m_width(width), m_height(height), m_window(nullptr), 
      m_textureID(0), m_fbo(0), m_lastTime(0.0), m_fpsAccum(0.0), m_fpsCount(0) {}

bool DisplayWindow::Init() {
    if (!glfwInit()) return false;
    

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_width, m_height, "Affichage Spot OBJ", nullptr, nullptr);
    if (!m_window) { glfwTerminate(); return false; }

    glfwMakeContextCurrent(m_window);
    
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureID, 0);

    m_lastTime = glfwGetTime();
    return true;
}

DisplayWindow::~DisplayWindow() {
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteTextures(1, &m_textureID);
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool DisplayWindow::ShouldClose() const { return glfwWindowShouldClose(m_window); }
void DisplayWindow::PollEvents() const { glfwPollEvents(); }

void DisplayWindow::RenderBuffer(const uint8_t* colorBuffer) {
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, colorBuffer);

    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, display_w, display_h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glfwSwapBuffers(m_window);
    UpdateFPS();
}

void DisplayWindow::UpdateFPS() {
    double now = glfwGetTime();
    double dt  = now - m_lastTime;
    m_lastTime = now;

    m_fpsAccum += dt;
    m_fpsCount++;

    if (m_fpsAccum >= 1.0) {
        double fps = (double)m_fpsCount / m_fpsAccum;
        double ms  = 1000.0 * m_fpsAccum / (double)m_fpsCount;

        char title[256];
        snprintf(title, sizeof(title),
                 "Affichage Spot OBJ | %.1f FPS (%.2f ms/frame)",
                 fps, ms);

        glfwSetWindowTitle(m_window, title);

        m_fpsAccum = 0.0;
        m_fpsCount = 0;
    }
}