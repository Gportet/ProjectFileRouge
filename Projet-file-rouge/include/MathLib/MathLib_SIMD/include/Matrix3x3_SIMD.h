#pragma once


#include "MathLib_SIMD/include/Vec3_SIMD.h"
#include <array>

namespace MathsSIMD
{
    class Matrix3x3_SIMD
    {
    public:
        __m128 rows[3];

        Matrix3x3_SIMD(); // Identity.
        explicit Matrix3x3_SIMD(const std::array<float, 9>& elements);
        Matrix3x3_SIMD(__m128 row0, __m128 row1, __m128 row2);

        static Matrix3x3_SIMD Identity();
        static Matrix3x3_SIMD Zero();

#pragma region Operators
        float& operator()(std::size_t row, std::size_t column);
        const float& operator()(std::size_t row, std::size_t column) const;

        Matrix3x3_SIMD operator+(const Matrix3x3_SIMD& rhs) const;
        Matrix3x3_SIMD operator-(const Matrix3x3_SIMD& rhs) const;
        Matrix3x3_SIMD operator*(const Matrix3x3_SIMD& rhs) const;

        Vec3_SIMD operator*(const Vec3_SIMD& rhs) const;
        Matrix3x3_SIMD operator*(float scalar) const;
        
        Matrix3x3_SIMD& operator*=(const Matrix3x3_SIMD& rhs);

        bool operator==(const Matrix3x3_SIMD& rhs) const;
        bool operator!=(const Matrix3x3_SIMD& rhs) const;

#pragma endregion
        Matrix3x3_SIMD Transpose() const;
        float Determinant() const;

        // Gauss-Jordan with scaled partial pivoting.
        // Rejects singular/ill-conditioned input.
        Matrix3x3_SIMD Inverse(
            float relativeTolerance = 64.0f * std::numeric_limits<float>::epsilon()
        ) const;

        static Matrix3x3_SIMD Scale(const Vec3_SIMD& scale);
        static Matrix3x3_SIMD RotationX(float radians);
        static Matrix3x3_SIMD RotationY(float radians);
        static Matrix3x3_SIMD RotationZ(float radians);
    
    };
}
