#include "MathLib_SIMD/include/Vec3_SIMD.h"
#include "MathLib_SIMD/include/Vec3_ASM.h"

namespace MathsSIMD
{
#pragma region Initialize
    Vec3_SIMD::Vec3_SIMD() : data {0,0,0}
    {
        
    }
    
    Vec3_SIMD::Vec3_SIMD(float x, float y, float z) noexcept : data { x, y, z } 
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
        if (rhs.data.x == 0.0f ||
            rhs.data.y == 0.0f ||
            rhs.data.z == 0.0f)
        {
            throw std::runtime_error("Division by zero");
        }
        
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
        
        const __m128 result = _mm_mul_ps (vecA, vScalar);
        const Vec3 vecResult = Store(result);
        
        return Vec3_SIMD(vecResult);
    }
    
    Vec3_SIMD Vec3_SIMD::operator/(float scalar) const
    {
        if (scalar == 0.0f)
        {
            throw std::runtime_error("Division by zero");
        }
        
        const __m128 vecA = Load(this->data);
        const __m128 vScalar = _mm_set1_ps(scalar);
        
        const __m128 result = _mm_div_ps (vecA, vScalar);
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
        
        vecA = _mm_mul_ps (vecA, vScalar);
        this->data = Store(vecA);
        
        return *this;
    }
    
    Vec3_SIMD& Vec3_SIMD::operator/=(float scalar)
    {
        __m128 vecA = Load(this->data);
        __m128 vScalar = _mm_set1_ps(scalar);
        
        vecA = _mm_div_ps (vecA, vScalar);
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
        return _mm_movemask_ps(result) != 0xF;
    }


#pragma endregion
    
#pragma region MathFormula 
    float Vec3_SIMD::Dot(const Vec3_SIMD& rhs) const
    {
        const __m128 vecA = Load(this->data);
        const __m128 vecB = Load(rhs.data);
        
        const __m128 result = _mm_mul_ps (vecA, vecB);
        
        float x = _mm_cvtss_f32(result);
        float y = _mm_cvtss_f32(_mm_shuffle_ps(result, result, _MM_SHUFFLE(1, 1, 1, 1)));
        float z = _mm_cvtss_f32(_mm_shuffle_ps(result, result, _MM_SHUFFLE(2, 2, 2, 2)));
        
        return x + y + z;
    }

    Vec3_SIMD Vec3_SIMD::Cross(const Vec3_SIMD& rhs) const
    {
        const __m128 vecA = Load(this->data);
        const __m128 vecB = Load(rhs.data);
        
        const __m128 vecA_yzx = _mm_shuffle_ps(vecA, vecA, _MM_SHUFFLE(3, 0, 2, 1));
        const __m128 vecB_zyx = _mm_shuffle_ps(vecB, vecB, _MM_SHUFFLE(3, 0, 1, 2));
        const __m128 result1 = _mm_mul_ps (vecA_yzx, vecB_zyx);
        
        const __m128 vecA_zyx = _mm_shuffle_ps(vecA, vecA, _MM_SHUFFLE(3, 0, 1, 2));
        const __m128 vecB_yzx = _mm_shuffle_ps(vecB, vecB, _MM_SHUFFLE(3, 0, 2, 1));
        const __m128 result2 = _mm_mul_ps (vecA_zyx, vecB_yzx);
        
        const __m128 resultFinal = _mm_sub_ps(result1, result2);
        
        return Vec3_SIMD(Store(resultFinal));
    }

    float Vec3_SIMD::MagnitudeSquared() const
    {
        return Dot(*this);
    }
    
    float Vec3_SIMD::Magnitude() const
    {
        const __m128 vecA = Load(this->data);
        
        const __m128 squared = _mm_mul_ps(vecA, vecA);
        
        // only y²
        const __m128 y_squared = _mm_shuffle_ps(vecA, vecA, _MM_SHUFFLE(1, 1, 1, 1));
        // only z²
        const __m128 z_squared = _mm_shuffle_ps(vecA, vecA, _MM_SHUFFLE(2, 2, 2, 2));
        
        // x² + y²
        const __m128 result1 = _mm_add_ps(squared, y_squared);
        
        // (x² + y²) + z²
        const __m128 finalResult = _mm_add_ps(result1, z_squared);
        
        // return first value
        return _mm_cvtss_f32(finalResult);
    }

    Vec3_SIMD Vec3_SIMD::Normalize() const
    {
        if (!std::isfinite(data.x) || !std::isfinite(data.y) || !std::isfinite(data.z))
        {
            throw std::domain_error("Cannot normalize non-finite components");
        }
        
        const float magnitude = Magnitude();
        
        if (magnitude == 0.0f)
        {
            throw std::domain_error("Cannot divide by zero");
        }
        
        const __m128 vecA = Load(this->data);
        const __m128 MagnitudeV = _mm_set1_ps(magnitude);
        
        const __m128 result = _mm_div_ps (vecA, MagnitudeV);
        
        return Vec3_SIMD(Store(result));
    }

    float Vec3_SIMD::DistanceSquared(const Vec3_SIMD& rhs) const
    {
        const __m128 vecA = Load(this->data);
        const __m128 vecB = Load(rhs.data);
        
        const __m128 diff = _mm_sub_ps (vecA, vecB);
        const __m128 squaredDiff = _mm_mul_ps (diff, diff);
        
        // only y²
        const __m128 squaredDiff_y = _mm_shuffle_ps(squaredDiff, squaredDiff, _MM_SHUFFLE(1, 1, 1, 1));
        // only z²
        const __m128 squaredDiff_z = _mm_shuffle_ps(squaredDiff, squaredDiff, _MM_SHUFFLE(2, 2, 2, 2));
        
        // x² + y²
        const __m128 squaredDiff_withY = _mm_add_ps(squaredDiff, squaredDiff_y);
        
        // (x² + y²) + z²
        const __m128 result = _mm_add_ps(squaredDiff_withY, squaredDiff_z);
        
        return _mm_cvtss_f32(result);
    }
    
    float Vec3_SIMD::Distance(const Vec3_SIMD& rhs) const
    {
        const __m128 vecA = Load(this->data);
        const __m128 vecB = Load(rhs.data);
        
        const __m128 diff = _mm_sub_ps (vecA, vecB);
        const __m128 squaredDiff = _mm_mul_ps (diff, diff);
        
        // only y²
        const __m128 squaredDiff_y = _mm_shuffle_ps(squaredDiff, squaredDiff, _MM_SHUFFLE(1, 1, 1, 1));
        // only z²
        const __m128 squaredDiff_z = _mm_shuffle_ps(squaredDiff, squaredDiff, _MM_SHUFFLE(2, 2, 2, 2));
        
        // x² + y²
        const __m128 squaredDiff_withY = _mm_add_ps(squaredDiff, squaredDiff_y);
        
        // (x² + y²) + z²
        const __m128 squaredDiff_withZ = _mm_add_ps(squaredDiff_withY, squaredDiff_z);
        
        const __m128 result = _mm_sqrt_ps(squaredDiff_withZ);
        
        return _mm_cvtss_f32(result);
    }

    float Vec3_SIMD::Angle(const Vec3_SIMD& rhs) const
    {
        const Vec3_SIMD normalizedA = Normalize();
        const Vec3_SIMD normalizedB = rhs.Normalize();
        
        Vec3_SIMD vecA (normalizedA);
        Vec3_SIMD vecB (normalizedB);
        
        const float cosine = vecA.Dot(vecB);
        
        if (!std::isfinite(cosine))
        {
            throw std::domain_error("Cannot calculate angle with non-finite cosine");
        }

        if (cosine < -1.0f || cosine > 1.0f)
        {
            throw std::domain_error("Cosine must be in the range [-1, 1]");
        }
        
        return static_cast<float>(std::acos(cosine));
    }

    Vec3_SIMD Vec3_SIMD::Lerp(const Vec3_SIMD& a, const Vec3_SIMD& b, float t)
    {
        const __m128 vecA = a.Load(a.data);
        const __m128 vecB = b.Load(b.data);
        
        
        const __m128 scalar = _mm_set1_ps(t);
        const __m128 oneMinusT = _mm_sub_ps(_mm_set1_ps(1.0f), scalar);
        
        const __m128 resultA = _mm_mul_ps (vecA, oneMinusT);
        const __m128 resultB = _mm_mul_ps (vecB, scalar);
        
        const __m128 result = _mm_add_ps (resultA, resultB);
        
        return Vec3_SIMD(a.Store(result));
    }

    Vec3_SIMD Vec3_SIMD::Min(const Vec3_SIMD& a, const Vec3_SIMD& b)
    {
        const __m128 vecA = a.Load(a.data);
        const __m128 vecB = b.Load(b.data);
        
        const __m128 minVec = _mm_min_ps(vecA, vecB);
        return Vec3_SIMD(a.Store(minVec));
    }

    Vec3_SIMD Vec3_SIMD::Max(const Vec3_SIMD& a, const Vec3_SIMD& b)
    {
        const __m128 vecA = a.Load(a.data);
        const __m128 vecB = b.Load(b.data);
        
        const __m128 maxVec = _mm_max_ps(vecA, vecB);
        return Vec3_SIMD(a.Store(maxVec)); 
    }

    const Vec3_SIMD Vec3_SIMD::Zero{0,0,0};
    
    
    const Vec3_SIMD Vec3_SIMD::One{1, 1, 1};
    
    
    const Vec3_SIMD Vec3_SIMD::UnitX{1, 0, 0};
    
    
    const Vec3_SIMD Vec3_SIMD::UnitY{0, 1, 0};
    
    
    const Vec3_SIMD Vec3_SIMD::UnitZ{0, 0, 1};


    float Vec3_SIMD::DotASM(const Vec3_SIMD& rhs) const
    {
        return DotVec3ASM(&this->data.x, &rhs.data.x);
    }
#pragma endregion
    
}
