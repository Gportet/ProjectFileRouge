#pragma once
#include "MathLib/MathLib_SIMD/include/Matrix4x4_SIMD.h"
#include "MathLib/MathLib_SIMD/include/Vec3_SIMD.h"

class Camera {
public:
    MathsSIMD::Vec3_SIMD eye;
    MathsSIMD::Vec3_SIMD target;
    MathsSIMD::Vec3_SIMD up;

    Camera(MathsSIMD::Vec3_SIMD e, MathsSIMD::Vec3_SIMD t, MathsSIMD::Vec3_SIMD u) 
        : eye(e), target(t), up(u) {}

    MathsSIMD::Matrix4x4_SIMD GetMVPMatrix(float time, int width, int height) const {
        MathsSIMD::Matrix4x4_SIMD model = MathsSIMD::Matrix4x4_SIMD::RotationY(time * 0.5f);
        return MathsSIMD::Matrix4x4_SIMD::MVP_Matrix(model, eye, target, up, 90, width, height);
    }
};