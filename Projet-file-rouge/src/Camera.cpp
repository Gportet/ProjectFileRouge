#include "Engine/Camera.h"

void Camera::UpdateCamera(const Input& input, float deltaTime)
{
    MathsSIMD::Vec3_SIMD forward = (target - eye).Normalize();
    MathsSIMD::Vec3_SIMD right = forward.Cross(up).Normalize();
    
    const float speed = 5.0f;
    const float movement = speed * deltaTime;
    
    if (input.IsKeyPressed(GLFW_KEY_W))
    {
        MathsSIMD::Vec3_SIMD D = forward * movement;
        eye += D;
        target += D;
    }
    
    if (input.IsKeyPressed(GLFW_KEY_S))
    {
        MathsSIMD::Vec3_SIMD D = - forward * movement;
        eye += D;
        target += D;
    }
    
    if (input.IsKeyPressed(GLFW_KEY_A))
    {
        MathsSIMD::Vec3_SIMD D = - right * movement;
        eye += D;
        target += D;
    }
    
    if (input.IsKeyPressed(GLFW_KEY_D))
    {
        MathsSIMD::Vec3_SIMD D = right * movement;
        eye += D;
        target += D;
    }
}
