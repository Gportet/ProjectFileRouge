#include "MathLib_SIMD/include/Vec3_SIMD.h"

#include <immintrin.h>


namespace MathsSIMD
{
#pragma region Initialize
    Vec3_SIMD::Vec3_SIMD() : data {0,0,0}
    {
        
    }
    
    Vec3_SIMD::Vec3_SIMD(float x, float y, float z) : data { x, y, z }
    {
        
    }
    
    Vec3_SIMD::Vec3_SIMD(const Vec3& vec) : data { static_cast<float> (vec.x), static_cast<float> (vec.y), static_cast<float> (vec.z) }
    {
        
    }
#pragma endregion

    __m128 Vec3_SIMD::Load(const Vec3& vecToSimd) const
    {
        return _mm_setr_ps(vecToSimd.x, vecToSimd.y, vecToSimd.z,0.f);
    }
    
    Vec3 Vec3_SIMD::Store(__m128 simdToVec) const
    {
        float components[4];
        _mm_storeu_ps(components, simdToVec);

        return {
            components[0],
            components[1],
            components[2],
        };
    }



#pragma region Operators 

    Vec3_SIMD Vec3_SIMD::operator+(const Vec3_SIMD& rhs) const
    {
        const __m128 vecA = Load(this->data);
        const __m128 vecB = Load(rhs.data);
        
        const __m128 result = _mm_add_ps (vecA, vecB);
        const Vec3 vecResult = Store(result);
        
        return Vec3_SIMD(vecResult);
    }
    
    Vec3_SIMD Vec3_SIMD::operator-(const Vec3_SIMD& rhs) const
    {
        const __m128 vecA = Load(this->data);
        const __m128 vecB = Load(rhs.data);
        
        const __m128 result = _mm_sub_ps (vecA, vecB);
        const Vec3 vecResult = Store(result);
        
        return Vec3_SIMD(vecResult);
    }
    
    Vec3_SIMD Vec3_SIMD::operator*(const Vec3_SIMD& rhs) const
    {
        const __m128 vecA = Load(this->data);
        const __m128 vecB = Load(rhs.data);
        
        const __m128 result = _mm_mul_ps (vecA, vecB);
        const Vec3 vecResult = Store(result);
        
        return Vec3_SIMD(vecResult);
    }
    
    Vec3_SIMD Vec3_SIMD::operator/(const Vec3_SIMD& rhs) const
    {
        const __m128 vecA = Load(this->data);
        const __m128 vecB = Load(rhs.data);
        
        const __m128 result = _mm_div_ps (vecA, vecB);
        const Vec3 vecResult = Store(result);
        
        return Vec3_SIMD(vecResult);
    }

    Vec3_SIMD Vec3_SIMD::operator-() const
    {
        const Vec3 nullVec { 0.f,0.f,0.f };
        const __m128 nullRegister = Load(nullVec);
        const __m128 vecA = Load(this->data);
        
        const __m128 minusVecA = _mm_sub_ps (nullRegister, vecA);
        const Vec3 vecResult = Store(minusVecA);
        
        return Vec3_SIMD(vecResult);
    }

    Vec3_SIMD Vec3_SIMD::operator*(float scalar) const
    {
        const __m128 vecA = Load(this->data);
        const __m128 vScalar = _mm_set1_ps(scalar);
        
        const __m128 result = _mm_mul_ps (vecA, vecA);
        const Vec3 vecResult = Store(result);
        
        return Vec3_SIMD(vecResult);
    }
    
    Vec3_SIMD Vec3_SIMD::operator/(float scalar) const
    {
        const __m128 vecA = Load(this->data);
        const __m128 vScalar = _mm_set1_ps(scalar);
        
        const __m128 result = _mm_div_ps (vecA, vecA);
        const Vec3 vecResult = Store(result);
        
        return Vec3_SIMD(vecResult);
    }

    Vec3_SIMD& Vec3_SIMD::operator+=(const Vec3_SIMD& rhs)
    {
        __m128 vecA = Load(this->data);
        __m128 vecB = Load(rhs.data);
        
        vecA = _mm_add_ps (vecA, vecB);
        this->data = Store(vecA);
        
        return *this;
    }
    
    Vec3_SIMD& Vec3_SIMD::operator-=(const Vec3_SIMD& rhs)
    {
        __m128 vecA = Load(this->data);
        __m128 vecB = Load(rhs.data);
        
        vecA = _mm_sub_ps (vecA, vecB);
        this->data = Store(vecA);
        
        return *this;
    }
    
    Vec3_SIMD& Vec3_SIMD::operator*=(const Vec3_SIMD& rhs)
    {
        __m128 vecA = Load(this->data);
        __m128 vecB = Load(rhs.data);
        
        vecA = _mm_mul_ps (vecA, vecB);
        this->data = Store(vecA);
        
        return *this;
    }
    
    Vec3_SIMD& Vec3_SIMD::operator/=(const Vec3_SIMD& rhs)
    {
        __m128 vecA = Load(this->data);
        __m128 vecB = Load(rhs.data);
        
        vecA = _mm_div_ps (vecA, vecB);
        this->data = Store(vecA);
        
        return *this;
    }

    Vec3_SIMD& Vec3_SIMD::operator*=(float scalar)
    {
        __m128 vecA = Load(this->data);
        __m128 vScalar = _mm_set1_ps(scalar);
        
        vecA = _mm_mul_ps (vecA, vecA);
        this->data = Store(vecA);
        
        return *this;
    }
    
    Vec3_SIMD& Vec3_SIMD::operator/=(float scalar)
    {
        __m128 vecA = Load(this->data);
        __m128 vScalar = _mm_set1_ps(scalar);
        
        vecA = _mm_div_ps (vecA, vecA);
        this->data = Store(vecA);
        
        return *this;
    }

    bool Vec3_SIMD::operator==(const Vec3_SIMD& rhs) const
    {
        const __m128 vecA = Load(this->data);
        const __m128 vecB = Load(rhs.data);
        
        const __m128 result = _mm_cmpeq_ps(vecA, vecB);
        
        return _mm_movemask_ps(result) == 0xF;
    }
    
    bool Vec3_SIMD::operator!=(const Vec3_SIMD& rhs) const
    {
        const __m128 vecA = Load(this->data);
        const __m128 vecB = Load(rhs.data);
        
        const __m128 result = _mm_cmpneq_ps(vecA, vecB);
        
        return _mm_movemask_ps(result) == 0xF;
    }


#pragma endregion
    
}
