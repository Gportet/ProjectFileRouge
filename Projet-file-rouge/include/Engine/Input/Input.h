#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Input
{
private:
    GLFWwindow* m_window;
    bool m_keys[GLFW_KEY_LAST + 1]{};
public:
    Input(GLFWwindow* window);
    
    void UpdateKeysPress();
    bool IsKeyPressed(int key) const;
};
