#pragma once
#include "MathLib/MathLib_SIMD/include/Vec3_SIMD.h"
#include "MathLib/MathLib_SIMD/include/Matrix4x4_SIMD.h"

namespace ECS
{
    // Les données sont de simples floats (Vec3) : on peut les lire et les modifier
    // directement (transform.rotation.y += ...). Les Vec3_SIMD ne servent qu'au calcul des matrices.
    struct TransformComponent
    {
        MathsSIMD::Vec3 position{0.0f, 0.0f, 0.0f};
        MathsSIMD::Vec3 rotation{0.0f, 0.0f, 0.0f};   // radians, autour de X, Y, Z
        MathsSIMD::Vec3 scale{1.0f, 1.0f, 1.0f};
        
        MathsSIMD::Matrix4x4_SIMD GetRotationMatrix() const
        {
            using MathsSIMD::Matrix4x4_SIMD;
            return Matrix4x4_SIMD::RotationZ(rotation.z) *
                   Matrix4x4_SIMD::RotationY(rotation.y) *
                   Matrix4x4_SIMD::RotationX(rotation.x);
        }
        
        MathsSIMD::Matrix4x4_SIMD GetModelMatrix() const
        {
            using MathsSIMD::Matrix4x4_SIMD;
            using MathsSIMD::Vec3_SIMD;
            return Matrix4x4_SIMD::Translation(Vec3_SIMD(position.x, position.y, position.z)) *
                   GetRotationMatrix() *
                   Matrix4x4_SIMD::Scale(Vec3_SIMD(scale.x, scale.y, scale.z));
        }
    };
}