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

    float Matrix3x3_SIMD::Determinant() const
    {
        const __m128 row1 = rows[1]; 
        const __m128 row2 = rows[2]; 
        
        const __m128 shuffleOne  = _mm_shuffle_ps(row1,row1, _MM_SHUFFLE(3, 0, 0, 1)); // E D D 0
        const __m128 shuffleTwo = _mm_shuffle_ps(row2,row2, _MM_SHUFFLE(3, 1, 2, 2)); // I I H 0
        const __m128 shuffleThree = _mm_shuffle_ps(row1,row1, _MM_SHUFFLE(3, 1, 2,2)); // F F E 0
        const __m128 shuffleFour = _mm_shuffle_ps(row2,row2, _MM_SHUFFLE(3, 0, 0, 1)); // H G G 0
        
        const __m128 firstProducts = _mm_mul_ps(shuffleOne, shuffleTwo);
        const __m128 secondProducts = _mm_mul_ps(shuffleThree, shuffleFour);
        
        const __m128 finalProduct = _mm_sub_ps(firstProducts, secondProducts);
        
        const __m128 row0 = rows[0];
        const __m128 products = _mm_mul_ps(row0, finalProduct);
        
        const __m128 signs = _mm_setr_ps(1.0f, -1.0f, 1.0f, 1.0f);
        const __m128 result = _mm_mul_ps(products, signs);
        
        const __m128 horizontal1 = _mm_hadd_ps(result, result);
        const __m128 horizontal2 = _mm_hadd_ps(horizontal1, horizontal1);

        return _mm_cvtss_f32(horizontal2);
    }

    Matrix3x3_SIMD Matrix3x3_SIMD::Inverse(float relativeTolerance) const
    {
        const __m128 row0 = rows[0];
        const __m128 row1 = rows[1];
        const __m128 row2 = rows[2];

        const __m128 shuffleOne =_mm_shuffle_ps(row1, row1, _MM_SHUFFLE(3, 0, 0, 1)); // [E D D 0]
        const __m128 shuffleTwo =_mm_shuffle_ps(row2, row2, _MM_SHUFFLE(3, 1, 2, 2)); // [I I H 0]
        const __m128 shuffleThree =_mm_shuffle_ps(row1, row1, _MM_SHUFFLE(3, 1, 2, 2)); // [F F E 0]
        const __m128 shuffleFour =_mm_shuffle_ps(row2, row2, _MM_SHUFFLE(3, 0, 0, 1)); // [H G G 0]
        const __m128 shuffleFive =_mm_shuffle_ps(row0, row0, _MM_SHUFFLE(3, 0, 0, 1)); // [B A A 0]
        const __m128 shuffleSix =_mm_shuffle_ps(row2, row2, _MM_SHUFFLE(3, 1, 2, 2)); // [I I H 0]
        const __m128 shuffleSeven =_mm_shuffle_ps(row0, row0, _MM_SHUFFLE(3, 1, 2, 2)); // [C C B 0]
        const __m128 shuffleEight =_mm_shuffle_ps(row2, row2, _MM_SHUFFLE(3, 0, 0, 1)); // [H G G 0]

        const __m128 shuffleNine =_mm_shuffle_ps(row0, row0, _MM_SHUFFLE(3, 0, 0, 1)); // [B A A 0]
        const __m128 shuffleTen =_mm_shuffle_ps(row1, row1, _MM_SHUFFLE(3, 1, 2, 2)); // [F F E 0]
        const __m128 shuffleEleven =_mm_shuffle_ps(row0, row0, _MM_SHUFFLE(3, 1, 2, 2)); // [C C B 0]
        const __m128 shuffleTwelve =_mm_shuffle_ps(row1, row1, _MM_SHUFFLE(3, 0, 0, 1)); // [E D D 0]

        const __m128 firstProducts =_mm_mul_ps(shuffleOne, shuffleTwo);
        const __m128 secondProducts =_mm_mul_ps(shuffleThree, shuffleFour);
        const __m128 firstCofactors =_mm_sub_ps(firstProducts, secondProducts);
        const __m128 thirdProducts =_mm_mul_ps(shuffleFive, shuffleSix);
        const __m128 fourthProducts =_mm_mul_ps(shuffleSeven, shuffleEight);
        const __m128 secondCofactors =_mm_sub_ps(thirdProducts, fourthProducts);
        const __m128 fifthProducts =_mm_mul_ps(shuffleNine, shuffleTen);
        const __m128 sixthProducts =_mm_mul_ps(shuffleEleven, shuffleTwelve);
        const __m128 thirdCofactors =_mm_sub_ps(fifthProducts, sixthProducts);
        const __m128 signs =_mm_setr_ps(1.0f, -1.0f, 1.0f, 1.0f);
        const __m128 cofactors0 =_mm_mul_ps(firstCofactors, signs);
        const __m128 cofactors1 =_mm_mul_ps(secondCofactors, signs);
        const __m128 cofactors2 =_mm_mul_ps(thirdCofactors, signs);
        const __m128 determinantProducts =_mm_mul_ps(row0, cofactors0);
        const __m128 determinantHorizontal1 =_mm_hadd_ps(determinantProducts, determinantProducts);
        const __m128 determinantHorizontal2 =_mm_hadd_ps(determinantHorizontal1, determinantHorizontal1);
        const float determinant =_mm_cvtss_f32(determinantHorizontal2);

        if (std::abs(determinant) <= relativeTolerance)
        {
            throw std::runtime_error("Matrix is not invertible.");
        }

        const __m128 cofactors01 =_mm_unpacklo_ps(cofactors0, cofactors1);
        // [C00 C10 C01 C11]

        const __m128 cofactors2Zero =_mm_unpacklo_ps(cofactors2, _mm_setzero_ps());
        // [C20 0 C21 0]

        const __m128 adjugate0 =_mm_shuffle_ps(cofactors01,cofactors2Zero,_MM_SHUFFLE(3, 0, 1, 0));
        // [C00 C10 C20 0]

        const __m128 adjugate1 =_mm_shuffle_ps(cofactors01,cofactors2Zero,_MM_SHUFFLE(3, 2, 3, 2));
        // [C01 C11 C21 0]

        const __m128 adjugate2 =_mm_shuffle_ps(cofactors0,cofactors2,_MM_SHUFFLE(3, 2, 3, 2));
        // [C02 C12 C22 0]

        const __m128 inverseDeterminant =_mm_set1_ps(1.0f / determinant);

        return {
            _mm_mul_ps(adjugate0, inverseDeterminant),
            _mm_mul_ps(adjugate1, inverseDeterminant),
            _mm_mul_ps(adjugate2, inverseDeterminant)
        };
    }

    Matrix3x3_SIMD Matrix3x3_SIMD::Scale(const Vec3_SIMD& scale)
    {
        const __m128 scaleRegister = scale.Load(scale.data);
        
        const __m128 row1 = _mm_shuffle_ps(scaleRegister,scaleRegister,_MM_SHUFFLE(3, 3, 3, 0));
        const __m128 row2 = _mm_shuffle_ps(scaleRegister,scaleRegister,_MM_SHUFFLE(3, 3, 1, 3));
        const __m128 row3 = _mm_shuffle_ps(scaleRegister,scaleRegister,_MM_SHUFFLE(3, 2, 3, 3));
        
        return {
            row1,
            row2,
            row3,
            };
    }
    
    Matrix3x3_SIMD Matrix3x3_SIMD::RotationX(float radians)
    {
        const float c = std::cos(radians);
        const float s = std::sin(radians);

        return {
            _mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f),
            _mm_setr_ps(0.0f, c, -s, 0.0f),
            _mm_setr_ps(0.0f, s, c, 0.0f)
        };
    }

    Matrix3x3_SIMD Matrix3x3_SIMD::RotationY(float radians)
    {
        const float c = std::cos(radians);
        const float s = std::sin(radians);

        return {
            _mm_setr_ps(c, 0.0f, s, 0.0f),
            _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f),
            _mm_setr_ps(-s, 0.0f, c, 0.0f)
        };
    }

    Matrix3x3_SIMD Matrix3x3_SIMD::RotationZ(float radians)
    {
        const float c = std::cos(radians);
        const float s = std::sin(radians);

        return {
            _mm_setr_ps(c, -s, 0.0f, 0.0f),
            _mm_setr_ps(s, c, 0.0f, 0.0f),
            _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f)
        };
    }
}
