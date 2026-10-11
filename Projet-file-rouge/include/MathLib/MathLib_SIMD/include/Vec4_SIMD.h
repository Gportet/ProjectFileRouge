#pragma once

#include <xmmintrin.h>
#include <stdexcept>

namespace MathsSIMD
{
    class Vec4_SIMD
    {
    public:        
        __m128 data;

        Vec4_SIMD();
        explicit Vec4_SIMD(__m128 value);

        
#pragma region Operators
        // Create a new Vec4 with 2 other
        Vec4_SIMD operator+(const Vec4_SIMD& rhs) const;
        Vec4_SIMD operator-(const Vec4_SIMD& rhs) const;
        Vec4_SIMD operator*(const Vec4_SIMD& rhs) const;
        Vec4_SIMD operator/(const Vec4_SIMD& rhs) const;

        // Negate the Vec4
        Vec4_SIMD operator-() const;

        Vec4_SIMD operator*(float scalar) const;
        Vec4_SIMD operator/(float scalar) const;

        // Modify "this" with another vector
        Vec4_SIMD& operator+=(const Vec4_SIMD& rhs);
        Vec4_SIMD& operator-=(const Vec4_SIMD& rhs);
        Vec4_SIMD& operator*=(const Vec4_SIMD& rhs);
        Vec4_SIMD& operator/=(const Vec4_SIMD& rhs);

        Vec4_SIMD& operator*=(float scalar);
        Vec4_SIMD& operator/=(float scalar);

        bool operator==(const Vec4_SIMD& rhs) const;
        bool operator!=(const Vec4_SIMD& rhs) const;
#pragma endregion

        float Dot(const Vec4_SIMD& rhs) const;
        float MagnitudeSquared() const;
        float Magnitude() const;
        // Returns a new vector. Throws domain_error for zero or non-finite input.
        Vec4_SIMD Normalize() const;
        float DistanceSquared(const Vec4_SIMD& rhs) const;
        float Distance(const Vec4_SIMD& rhs) const;
        float Angle(const Vec4_SIMD& rhs) const;
        static Vec4_SIMD Lerp(const Vec4_SIMD& a, const Vec4_SIMD& b, float t);
        static Vec4_SIMD Min(const Vec4_SIMD& a, const Vec4_SIMD& b);
        static Vec4_SIMD Max(const Vec4_SIMD& a, const Vec4_SIMD& b);

        static const Vec4_SIMD Zero;
        static const Vec4_SIMD One;
        static const Vec4_SIMD UnitX;
        static const Vec4_SIMD UnitY;
        static const Vec4_SIMD UnitZ;
        static const Vec4_SIMD UnitW;
    
    };
}