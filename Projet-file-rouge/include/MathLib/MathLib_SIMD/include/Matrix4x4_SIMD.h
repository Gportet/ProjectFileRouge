#pragma once

#include "MathLib_SIMD/include/Vec4_SIMD.h"
#include "MathLib_SIMD/include/Vec3_SIMD.h"
#include <array>
#include <cstddef>
#include <limits>

namespace MathsSIMD
{
    class Matrix4x4_SIMD
    {
    public:
        __m128 rows[4];

        Matrix4x4_SIMD(); // Identity.
        explicit Matrix4x4_SIMD(const std::array<float, 16>& elements);
        Matrix4x4_SIMD(__m128 row0, __m128 row1, __m128 row2, __m128 row3);

        static Matrix4x4_SIMD Identity();
        static Matrix4x4_SIMD Zero();

#pragma region Operators
        float& operator()(std::size_t row, std::size_t column);
        const float& operator()(std::size_t row, std::size_t column) const;

        Matrix4x4_SIMD operator+(const Matrix4x4_SIMD& rhs) const;
        Matrix4x4_SIMD operator-(const Matrix4x4_SIMD& rhs) const;
        Matrix4x4_SIMD operator*(const Matrix4x4_SIMD& rhs) const;

        Vec4_SIMD operator*(const Vec4_SIMD& rhs) const;
        Matrix4x4_SIMD operator*(float scalar) const;

        Matrix4x4_SIMD& operator*=(const Matrix4x4_SIMD& rhs);

        bool operator==(const Matrix4x4_SIMD& rhs) const;
        bool operator!=(const Matrix4x4_SIMD& rhs) const;

#pragma endregion

        Matrix4x4_SIMD Transpose() const;
        float Determinant() const;

        // Gauss-Jordan with scaled partial pivoting.
        // Rejects singular/ill-conditioned input.
        Matrix4x4_SIMD Inverse(
            float relativeTolerance = 64.0f * std::numeric_limits<float>::epsilon()
        ) const;

        static Matrix4x4_SIMD Scale(const Vec3_SIMD& scale);

        static Matrix4x4_SIMD RotationX(float radians);
        static Matrix4x4_SIMD RotationY(float radians);
        static Matrix4x4_SIMD RotationZ(float radians);

        static Matrix4x4_SIMD Translation(const Vec3_SIMD& offset);

        Vec3_SIMD TransformPoint(const Vec3_SIMD& point) const; // Affine only, w = 1.
        Vec3_SIMD TransformDirection(const Vec3_SIMD& direction) const; // Affine only, w = 0.
    };
}