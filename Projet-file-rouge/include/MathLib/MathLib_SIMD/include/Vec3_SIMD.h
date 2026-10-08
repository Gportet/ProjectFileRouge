#pragma once

#include <xmmintrin.h>

namespace MathsSIMD
{
    struct Vec3
    {
        float x, y, z;
    };
    
    class Vec3_SIMD
    {
    public:
        Vec3 data;
        
        Vec3_SIMD();
        Vec3_SIMD(float x, float y, float z);
        explicit Vec3_SIMD(const Vec3& vec);
        

        
#pragma region Operators
        
        // Convert Vec3 -> __m128 & __m128 -> Vec3
        inline __m128 Load (const Vec3& vecToSimd) const;
        inline Vec3 Store (__m128 simdToVec) const;
        
        // Create a new Vec3 with 2 other
        inline Vec3_SIMD operator+(const Vec3_SIMD& rhs) const;
        inline Vec3_SIMD operator-(const Vec3_SIMD& rhs) const;
        inline Vec3_SIMD operator*(const Vec3_SIMD& rhs) const;
        inline Vec3_SIMD operator/(const Vec3_SIMD& rhs) const;
        
        // Negate the Vec3 
        inline Vec3_SIMD operator-() const;
        
        inline Vec3_SIMD operator*(float scalar) const;
        inline Vec3_SIMD operator/(float scalar) const;
        
        // Modify "this" with another vector
        inline Vec3_SIMD& operator+=(const Vec3_SIMD& rhs);
        inline Vec3_SIMD& operator-=(const Vec3_SIMD& rhs);
        inline Vec3_SIMD& operator*=(const Vec3_SIMD& rhs);
        inline Vec3_SIMD& operator/=(const Vec3_SIMD& rhs);
        
        inline Vec3_SIMD& operator*=(float scalar);
        inline Vec3_SIMD& operator/=(float scalar);
        
        bool operator==(const Vec3_SIMD& rhs) const;
        bool operator!=(const Vec3_SIMD& rhs) const;
#pragma endregion
    
    };
}
