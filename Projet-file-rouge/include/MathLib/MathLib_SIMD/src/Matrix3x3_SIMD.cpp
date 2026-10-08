#include "MathLib_SIMD/include/Matrix3x3_SIMD.h"


namespace MathsSIMD
{
    Matrix3x3_SIMD::Matrix3x3_SIMD() : 
    rows{        
        _mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f),
        _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f),
        _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f)
        }
    {
        
    }

    Matrix3x3_SIMD::Matrix3x3_SIMD(const std::array<float, 9>& elements)
    {
        rows[0] = _mm_setr_ps(
                elements[0],
                elements[1],
                elements[2],
                0.0f
                );

        rows[1] = _mm_setr_ps(
                elements[3],
                elements[4],
                elements[5],
                0.0f
                );

        rows[2] = _mm_setr_ps(
                elements[6],
                elements[7],
                elements[8],
                0.0f
                );
    }

    Matrix3x3_SIMD Matrix3x3_SIMD::Identity()
    {
        return Matrix3x3_SIMD();
    }

    Matrix3x3_SIMD Matrix3x3_SIMD::Zero()
    {
        Matrix3x3_SIMD result;

        result.rows[0] = _mm_setzero_ps();
        result.rows[1] = _mm_setzero_ps();
        result.rows[2] = _mm_setzero_ps();

        return result;
    }

    Matrix3x3_SIMD::Matrix3x3_SIMD(__m128 row0,__m128 row1,__m128 row2): rows{row0, row1, row2}
    {
    }
    
#pragma region Operators
    float& Matrix3x3_SIMD::operator()(std::size_t row, std::size_t column)
    {
        if (row >= 3 || column >= 3)
        {
            throw std::out_of_range("Matrix index is out of range");
        }
        
        return reinterpret_cast<float*>(&rows[row])[column];
        
    }
    
    const float& Matrix3x3_SIMD::operator()(std::size_t row, std::size_t column) const
    {
        if (row >= 3 || column >= 3)
        {
            throw std::out_of_range("Matrix index is out of range");
        }

        return reinterpret_cast<const float*>(&rows[row])[column];
    }

    Matrix3x3_SIMD Matrix3x3_SIMD::operator+(const Matrix3x3_SIMD& rhs) const
    {
        const __m128 row0 = _mm_add_ps(rows[0], rhs.rows[0]);
        const __m128 row1 = _mm_add_ps(rows[1], rhs.rows[1]);
        const __m128 row2 = _mm_add_ps(rows[2], rhs.rows[2]);
        
        Matrix3x3_SIMD resultMatrix;
        resultMatrix.rows[0] = row0;
        resultMatrix.rows[1] = row1;
        resultMatrix.rows[2] = row2;
        
        return resultMatrix;
    }
    
    Matrix3x3_SIMD Matrix3x3_SIMD::operator-(const Matrix3x3_SIMD& rhs) const
    {
        const __m128 row0 = _mm_sub_ps(rows[0], rhs.rows[0]);
        const __m128 row1 = _mm_sub_ps(rows[1], rhs.rows[1]);
        const __m128 row2 = _mm_sub_ps(rows[2], rhs.rows[2]);
        
        Matrix3x3_SIMD resultMatrix;
        resultMatrix.rows[0] = row0;
        resultMatrix.rows[1] = row1;
        resultMatrix.rows[2] = row2;
        
        return resultMatrix;
    }
    
    Matrix3x3_SIMD Matrix3x3_SIMD::operator*( const Matrix3x3_SIMD& rhs) const
    {
        Matrix3x3_SIMD result = Zero();

        for (std::size_t row = 0; row < 3; ++row)
        {
            for (std::size_t column = 0; column < 3; ++column)
            {
                for (std::size_t k = 0; k < 3; ++k)
                {
                    result(row, column) += (*this)(row, k) * rhs(k, column);
                }
            }
        }

        return result;
    }

    Vec3_SIMD Matrix3x3_SIMD::operator*(const Vec3_SIMD& rhs) const
    {
        const __m128 vecRhs = rhs.Load(rhs.data);
        
        const __m128 rowOneResult = _mm_mul_ps(rows[0], vecRhs);
        const __m128 rowTwoResult = _mm_mul_ps(rows[1], vecRhs);
        const __m128 rowThreeResult = _mm_mul_ps(rows[2], vecRhs);
        
        const __m128 row1XYResult = _mm_hadd_ps(rowOneResult, rowOneResult);
        const __m128 row1FinalResult = _mm_hadd_ps(row1XYResult, row1XYResult);
        
        const __m128 row2XYResult = _mm_hadd_ps(rowTwoResult, rowTwoResult);
        const __m128 row2FinalResult = _mm_hadd_ps(row2XYResult, row2XYResult);
        
        const __m128 row3XYResult = _mm_hadd_ps(rowThreeResult, rowThreeResult);
        const __m128 row3FinalResult = _mm_hadd_ps(row3XYResult, row3XYResult);
        
        const float x = _mm_cvtss_f32(row1FinalResult);
        const float y = _mm_cvtss_f32(row2FinalResult);
        const float z = _mm_cvtss_f32(row3FinalResult);

        return Vec3_SIMD(x, y, z);
        
    }

    Matrix3x3_SIMD Matrix3x3_SIMD::operator*(float scalar) const
    {
        const __m128 scalarSIMD = _mm_set1_ps(scalar);
        
        const __m128 rowOneResult = _mm_mul_ps(rows[0], scalarSIMD);
        const __m128 rowTwoResult = _mm_mul_ps(rows[1], scalarSIMD);
        const __m128 rowThreeResult = _mm_mul_ps(rows[2], scalarSIMD);
        
        return {
            rowOneResult,
            rowTwoResult,
            rowThreeResult
        };
    }

    Matrix3x3_SIMD& Matrix3x3_SIMD::operator*=(
        const Matrix3x3_SIMD& rhs)
    {
        Matrix3x3_SIMD result = Zero();

        for (std::size_t row = 0; row < 3; ++row)
        {
            for (std::size_t column = 0; column < 3; ++column)
            {
                for (std::size_t k = 0; k < 3; ++k)
                {
                    result(row, column) +=
                        (*this)(row, k) * rhs(k, column);
                }
            }
        }

        *this = result;
        return *this;
    }
    
    bool Matrix3x3_SIMD::operator==(const Matrix3x3_SIMD& rhs) const
    {
        for (std::size_t row = 0; row < 3; ++row)
        {
            const __m128 cmp = _mm_cmpeq_ps(rows[row], rhs.rows[row]);

            if (_mm_movemask_ps(cmp) != 0xF)
            {
                return false;
            }
        }

        return true;
    }
    
    bool Matrix3x3_SIMD::operator!=(const Matrix3x3_SIMD& rhs) const
    {
        return !(*this == rhs);
    }
#pragma endregion 
    
    Matrix3x3_SIMD Matrix3x3_SIMD::Transpose() const
    {
        const __m128 row0 = rows[0];
        const __m128 row1 = rows[1];
        const __m128 row2 = rows[2];

        // [m00, m01, m10, m11]
        const __m128 tmp0 = _mm_shuffle_ps(row0,row1,_MM_SHUFFLE(1, 0, 1, 0));

        // [m02, 0, m12, 0]
        const __m128 tmp1 = _mm_shuffle_ps(row0,row1,_MM_SHUFFLE(3, 2, 3, 2));

        // [m00, m10, m20, 0]
        const __m128 result0 = _mm_shuffle_ps(tmp0,row2,_MM_SHUFFLE(3, 0, 2, 0));

        // [m01, m11, m21, 0]
        const __m128 result1 = _mm_shuffle_ps(tmp0,row2,_MM_SHUFFLE(3, 1, 3, 1));

        // [m02, m12, m22, 0]
        const __m128 result2 = _mm_shuffle_ps(tmp1,row2,_MM_SHUFFLE(3, 2, 2, 0));

        return Matrix3x3_SIMD(result0, result1, result2);
    }
}
