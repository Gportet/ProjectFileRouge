#include "MathLib_SIMD/include/Vec4_SIMD.h"

#include "Vec4.h"

namespace MathsSIMD
{
#pragma region Initialize
    Vec4_SIMD::Vec4_SIMD() : data(_mm_setzero_ps())
    {
    }

    Vec4_SIMD::Vec4_SIMD(__m128 value) : data {value}
    {
        
    }
#pragma endregion
    
    #pragma region Operators

    Vec4_SIMD Vec4_SIMD::operator+(const Vec4_SIMD& rhs) const
    {
        const __m128 result = _mm_add_ps(this->data, rhs.data);
        return Vec4_SIMD(result);
    }

    Vec4_SIMD Vec4_SIMD::operator-(const Vec4_SIMD& rhs) const
    {
        const __m128 result = _mm_sub_ps(this->data, rhs.data);
        return Vec4_SIMD(result);
    }

    Vec4_SIMD Vec4_SIMD::operator*(const Vec4_SIMD& rhs) const
    {
        const __m128 result = _mm_mul_ps(this->data, rhs.data);
        return Vec4_SIMD(result);
    }

    Vec4_SIMD Vec4_SIMD::operator/(const Vec4_SIMD& rhs) const
    {
        const __m128 zero = _mm_setzero_ps();
        const __m128 comparison = _mm_cmpeq_ps(rhs.data, zero);

        if (_mm_movemask_ps(comparison) != 0)
        {
            throw std::runtime_error("Division by zero.");
        }
        
        const __m128 result = _mm_div_ps(this->data, rhs.data);
        return Vec4_SIMD(result);
    }

    Vec4_SIMD Vec4_SIMD::operator-() const
    {
        const __m128 zero = _mm_setzero_ps();
        const __m128 minusVec = _mm_sub_ps(zero, this->data);
        
        return Vec4_SIMD(minusVec);
    }

    Vec4_SIMD Vec4_SIMD::operator*(float scalar) const
    {
        const __m128 scalarVec = _mm_set1_ps(scalar);
        const __m128 result = _mm_mul_ps(this->data, scalarVec);
        
        return Vec4_SIMD(result);
    }

    Vec4_SIMD Vec4_SIMD::operator/(float scalar) const
    {
        if (scalar == 0.0f)
        {
            throw std::runtime_error("Division by zero");
        }
        
        const __m128 vScalar = _mm_set1_ps(scalar);
        const __m128 result = _mm_div_ps (this->data, vScalar);
        
        return Vec4_SIMD(result);
    }

    Vec4_SIMD& Vec4_SIMD::operator+=(const Vec4_SIMD& rhs)
    {
        this->data = _mm_add_ps (this->data, rhs.data);
        return *this;
    }

    Vec4_SIMD& Vec4_SIMD::operator-=(const Vec4_SIMD& rhs)
    {
        this->data = _mm_sub_ps (this->data, rhs.data);
        return *this;
    }

    Vec4_SIMD& Vec4_SIMD::operator*=(const Vec4_SIMD& rhs)
    {
        this->data = _mm_mul_ps (this->data, rhs.data);
        return *this;
    }

    Vec4_SIMD& Vec4_SIMD::operator/=(const Vec4_SIMD& rhs)
    {
        const __m128 zero = _mm_setzero_ps();
        const __m128 comparison = _mm_cmpeq_ps(rhs.data, zero);

        if (_mm_movemask_ps(comparison) != 0)
        {
            throw std::runtime_error("Division by zero.");
        }

        this->data = _mm_div_ps(this->data, rhs.data);
        return *this;
    }

    Vec4_SIMD& Vec4_SIMD::operator*=(float scalar)
    {
        const __m128 scalarVec = _mm_set1_ps(scalar);
        this->data = _mm_mul_ps(this->data, scalarVec);
        return *this;
    }

    Vec4_SIMD& Vec4_SIMD::operator/=(float scalar)
    {
        if (scalar == 0.0f)
        {
            throw std::runtime_error("Division by zero");
        }
        
        const __m128 scalarVec = _mm_set1_ps(scalar);
        this->data = _mm_div_ps(this->data, scalarVec);
        return *this;
    }

    bool Vec4_SIMD::operator==(const Vec4_SIMD& rhs) const
    {
        const __m128 result = _mm_cmpeq_ps(this->data, rhs.data);
        return _mm_movemask_ps(result) == 0xF;
    }

    bool Vec4_SIMD::operator!=(const Vec4_SIMD& rhs) const
    {
        const __m128 result = _mm_cmpeq_ps(this->data, rhs.data);
        return _mm_movemask_ps(result) != 0xF;
    }

#pragma endregion

#pragma region MathFormula

    float Vec4_SIMD::Dot(const Vec4_SIMD& rhs) const
    {
        const __m128 result = _mm_mul_ps (this->data, rhs.data);
        
        float x = _mm_cvtss_f32(result);
        float y = _mm_cvtss_f32(_mm_shuffle_ps(result, result, _MM_SHUFFLE(1, 1, 1, 1)));
        float z = _mm_cvtss_f32(_mm_shuffle_ps(result, result, _MM_SHUFFLE(2, 2, 2, 2)));
        float w = _mm_cvtss_f32(_mm_shuffle_ps(result, result, _MM_SHUFFLE(3, 3, 3, 3)));
        
        return x + y + z + w;
    }

    float Vec4_SIMD::MagnitudeSquared() const
    {
        const __m128 squared = _mm_mul_ps(this->data, this->data);
        const __m128 sum1 = _mm_hadd_ps(squared, squared);
        const __m128 sum2 = _mm_hadd_ps(sum1, sum1);

        return _mm_cvtss_f32(sum2);
    }

    float Vec4_SIMD::Magnitude() const
    {
        return std::sqrt(MagnitudeSquared());
    }

    Vec4_SIMD Vec4_SIMD::Normalize() const
    {
        const __m128 finiteMask = _mm_cmpord_ps(data, data);

        if (_mm_movemask_ps(finiteMask) != 0xF)
        {
            throw std::domain_error("Cannot normalize non-finite components");
        }

        const float magnitude = Magnitude();

        if (magnitude == 0.0f)
        {
            throw std::domain_error("Cannot divide by zero");
        }

        const __m128 magnitudeV = _mm_set1_ps(magnitude);

        return Vec4_SIMD(_mm_div_ps(data, magnitudeV));
    }

    float Vec4_SIMD::DistanceSquared(const Vec4_SIMD& rhs) const
    {
        const __m128 diff = _mm_sub_ps(data, rhs.data);
        const __m128 squared = _mm_mul_ps(diff, diff);

        const __m128 sum1 = _mm_hadd_ps(squared, squared);
        const __m128 sum2 = _mm_hadd_ps(sum1, sum1);

        return _mm_cvtss_f32(sum2);
    }

    float Vec4_SIMD::Distance(const Vec4_SIMD& rhs) const
    {
        return std::sqrt(DistanceSquared(rhs));
    }
    
    float Vec4_SIMD::Angle(const Vec4_SIMD& rhs) const
    {
        const Vec4_SIMD normalizedA = Normalize();
        const Vec4_SIMD normalizedB = rhs.Normalize();

        const float cosine = normalizedA.Dot(normalizedB);

        if (!std::isfinite(cosine))
        {
            throw std::domain_error("Cannot calculate angle with non-finite cosine");
        }

        const float clampedCosine = std::clamp(cosine, -1.0f, 1.0f);

        return std::acos(clampedCosine);
    }

    Vec4_SIMD Vec4_SIMD::Lerp(const Vec4_SIMD& a, const Vec4_SIMD& b, float t)
    {
        const __m128 scalar = _mm_set1_ps(t);
        const __m128 oneMinusT = _mm_sub_ps(_mm_set1_ps(1.0f), scalar);

        const __m128 resultA = _mm_mul_ps(a.data, oneMinusT);
        const __m128 resultB = _mm_mul_ps(b.data, scalar);

        const __m128 result = _mm_add_ps(resultA, resultB);

        return Vec4_SIMD(result);
    }

    Vec4_SIMD Vec4_SIMD::Min(const Vec4_SIMD& a, const Vec4_SIMD& b)
    {
        const __m128 vecA = a.data;
        const __m128 vecB = b.data;
        
        const __m128 minVec = _mm_min_ps(vecA, vecB);
        return Vec4_SIMD(minVec);
    }

    Vec4_SIMD Vec4_SIMD::Max(const Vec4_SIMD& a, const Vec4_SIMD& b)
    {
        const __m128 vecA = a.data;
        const __m128 vecB = b.data;
        
        const __m128 minVec = _mm_max_ps(vecA, vecB);
        return Vec4_SIMD(minVec);
    }

    const Vec4_SIMD Vec4_SIMD::Zero{_mm_setr_ps(0.0f, 0.0f, 0.0f, 0.0f)};

    const Vec4_SIMD Vec4_SIMD::One{_mm_setr_ps(1.0f, 1.0f, 1.0f, 1.0f)};

    const Vec4_SIMD Vec4_SIMD::UnitX{_mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f)};

    const Vec4_SIMD Vec4_SIMD::UnitY{_mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f)};

    const Vec4_SIMD Vec4_SIMD::UnitZ{_mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f)};

    const Vec4_SIMD Vec4_SIMD::UnitW{_mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f)};

#pragma endregion
}
