#pragma once
#include "ECS/ComponentManager.h"
#include "ECS/EntityManager.h"
#include "ECS/Component/CameraComponent.h"
#include "Engine/InputDirectory/Input.h"

namespace ECS
{
    inline void CameraMovementSystem(ECS::EntityManager& entityManager, Input& input, float dt)
    {
        entityManager.ForAllEntities(MakeMask<CameraComponent>(), [&](Entity e)
        {
            CameraComponent& transform = componentManager.getComponent<CameraComponent>(e);
            const float speed = 5.0f;
            const float movement = speed * dt;
            
            MathsSIMD::Vec3_SIMD forward = (transform.target - transform.eye).Normalize();
            MathsSIMD::Vec3_SIMD right = forward.Cross(transform.up).Normalize();
            
            if (input.IsKeyPressed(GLFW_KEY_W))
            {
                MathsSIMD::Vec3_SIMD D = forward * movement;
                transform.eye += D;
                transform.target += D;
            }
    
            if (input.IsKeyPressed(GLFW_KEY_S))
            {
                MathsSIMD::Vec3_SIMD D = - forward * movement;
                transform.eye += D;
                transform.target += D;
            }
    
            if (input.IsKeyPressed(GLFW_KEY_A))
            {
                MathsSIMD::Vec3_SIMD D = - right * movement;
                transform.eye += D;
                transform.target += D;
            }
    
            if (input.IsKeyPressed(GLFW_KEY_D))
            {
                MathsSIMD::Vec3_SIMD D = right * movement;
                transform.eye += D;
                transform.target += D;
            }
            
            if (input.IsKeyPressed(GLFW_KEY_SPACE))
            {
                MathsSIMD::Vec3_SIMD D = transform.up * movement;
                transform.eye += D;
                transform.target += D;
            }
            
            if (input.IsKeyPressed(GLFW_KEY_LEFT_CONTROL))
            {
                MathsSIMD::Vec3_SIMD D = transform.up * movement;
                transform.eye -= D;
                transform.target -= D;
            }
        }
        );
    }
}
