#pragma once

#include <xmmintrin.h>
#include <cmath>
#include <stdexcept>

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
        Vec3_SIMD(float x, float y, float z) noexcept;
        explicit Vec3_SIMD(const Vec3& vec);
        

        
#pragma region Operators
        
        // Convert Vec3 -> __m128 & __m128 -> Vec3
         __m128 Load (const Vec3& vecToSimd) const;
         Vec3 Store (__m128 simdToVec) const;
        
        // Create a new Vec3 with 2 other
         Vec3_SIMD operator+(const Vec3_SIMD& rhs) const;
         Vec3_SIMD operator-(const Vec3_SIMD& rhs) const;
         Vec3_SIMD operator*(const Vec3_SIMD& rhs) const;
         Vec3_SIMD operator/(const Vec3_SIMD& rhs) const;
        
        // Negate the Vec3 
         Vec3_SIMD operator-() const;
        
         Vec3_SIMD operator*(float scalar) const;
         Vec3_SIMD operator/(float scalar) const;
        
        // Modify "this" with another vector
         Vec3_SIMD& operator+=(const Vec3_SIMD& rhs);
         Vec3_SIMD& operator-=(const Vec3_SIMD& rhs);
         Vec3_SIMD& operator*=(const Vec3_SIMD& rhs);
         Vec3_SIMD& operator/=(const Vec3_SIMD& rhs);
        
         Vec3_SIMD& operator*=(float scalar);
         Vec3_SIMD& operator/=(float scalar);
        
        bool operator==(const Vec3_SIMD& rhs) const;
        bool operator!=(const Vec3_SIMD& rhs) const;
#pragma endregion
        
#pragma region MathFormula 
        float Dot (const Vec3_SIMD& rhs) const;
        Vec3_SIMD Cross (const Vec3_SIMD& rhs) const;
        float MagnitudeSquared() const;
        float Magnitude() const;
        
        Vec3_SIMD Normalize () const;
        float DistanceSquared (const Vec3_SIMD& rhs) const;
        float Distance (const Vec3_SIMD& rhs) const;
        float Angle (const Vec3_SIMD& rhs) const;
        
        static Vec3_SIMD Lerp(const Vec3_SIMD& a, const Vec3_SIMD& b, float t);
        static Vec3_SIMD Min(const Vec3_SIMD& a, const Vec3_SIMD& b);
        static Vec3_SIMD Max(const Vec3_SIMD& a, const Vec3_SIMD& b);

        static const Vec3_SIMD Zero;
        static const Vec3_SIMD One;
        static const Vec3_SIMD UnitX;
        static const Vec3_SIMD UnitY;
        static const Vec3_SIMD UnitZ;
        
        float DotASM(const Vec3_SIMD& rhs) const;
#pragma endregion
    
    };
}
