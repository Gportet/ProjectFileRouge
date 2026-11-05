#include "Engine/Input/Input.h"

Input::Input(GLFWwindow* window)
    : m_window(window)
{
}

void Input::UpdateKeysPress()
{
    const int keys[] = {
        GLFW_KEY_W,
        GLFW_KEY_A,
        GLFW_KEY_S,
        GLFW_KEY_D,
        GLFW_KEY_SPACE,
        GLFW_KEY_LEFT_CONTROL
    };
    
    for (int i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
    {
        m_keys[keys[i]] = glfwGetKey(m_window, keys[i]) == GLFW_PRESS;
    }
}

bool Input::IsKeyPressed(int key) const
{
    if (key < 0 || key > GLFW_KEY_LAST)
        return false;
    
    return m_keys[key];
}
