#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class DisplayWindow {
private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    GLuint m_textureID;
    GLuint m_fbo;
    
    double m_lastTime;
    double m_fpsAccum;
    int m_fpsCount;

public:
    DisplayWindow(int width, int height, const std::string& title);
    ~DisplayWindow();

    bool Init();
    bool ShouldClose() const;
    void PollEvents() const;
    void RenderBuffer(const uint8_t* colorBuffer);
    void UpdateFPS();
    
    double GetTime() const { return glfwGetTime(); }
};