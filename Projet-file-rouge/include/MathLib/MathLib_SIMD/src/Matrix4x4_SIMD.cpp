#include "MathLib_SIMD/include/Matrix4x4_SIMD.h"

namespace MathsSIMD
{
    Matrix4x4_SIMD::Matrix4x4_SIMD() : rows{        
        _mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f),
        _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f),
        _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f),
        _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f)
        }
    {
        
    }

    Matrix4x4_SIMD::Matrix4x4_SIMD(const std::array<float, 16>& elements)
    {
        rows[0] = _mm_setr_ps(
            elements[0],
            elements[1],
            elements[2],
            elements[3]
        );

        rows[1] = _mm_setr_ps(
            elements[4],
            elements[5],
            elements[6],
            elements[7]
        );

        rows[2] = _mm_setr_ps(
            elements[8],
            elements[9],
            elements[10],
            elements[11]
        );

        rows[3] = _mm_setr_ps(
            elements[12],
            elements[13],
            elements[14],
            elements[15]
        );
    }

    Matrix4x4_SIMD::Matrix4x4_SIMD(__m128 row0,__m128 row1,__m128 row2,__m128 row3) : rows {row0,row1,row2,row3}
    {
        
    }

    Matrix4x4_SIMD Matrix4x4_SIMD::Identity()
    {
        return Matrix4x4_SIMD();
    }

    Matrix4x4_SIMD Matrix4x4_SIMD::Zero()
    {
        Matrix4x4_SIMD result;

        result.rows[0] = _mm_setzero_ps();
        result.rows[1] = _mm_setzero_ps();
        result.rows[2] = _mm_setzero_ps();
        result.rows[3] = _mm_setzero_ps();

        return result;
    }


#pragma region Operators

    float& Matrix4x4_SIMD::operator()(std::size_t row,std::size_t column)
    {
        if (row >= 4 || column >= 4)
        {
            throw std::out_of_range("Matrix index is out of range");
        }

        return reinterpret_cast<float*>(&this->rows[row])[column]; 
    }

    const float& Matrix4x4_SIMD::operator()(std::size_t row,std::size_t column) const
    {
        if (row >= 4 || column >= 4)
        {
            throw std::out_of_range("Matrix index is out of range");
        }

        return reinterpret_cast<const float*>(&this->rows[row])[column];
    }

    Matrix4x4_SIMD Matrix4x4_SIMD::operator+(const Matrix4x4_SIMD& rhs) const
    {
        const __m128 row0 = _mm_add_ps(this->rows[0], rhs.rows[0]);
        const __m128 row1 = _mm_add_ps(this->rows[1], rhs.rows[1]);
        const __m128 row2 = _mm_add_ps(this->rows[2], rhs.rows[2]);
        const __m128 row3 = _mm_add_ps(this->rows[3], rhs.rows[3]);
        
        Matrix4x4_SIMD resultMatrix;
        resultMatrix.rows[0] = row0;
        resultMatrix.rows[1] = row1;
        resultMatrix.rows[2] = row2;
        resultMatrix.rows[3] = row3;
        
        return resultMatrix;
    }

    Matrix4x4_SIMD Matrix4x4_SIMD::operator-(
        const Matrix4x4_SIMD& rhs) const
    {
        const __m128 row0 = _mm_sub_ps(this->rows[0], rhs.rows[0]);
        const __m128 row1 = _mm_sub_ps(this->rows[1], rhs.rows[1]);
        const __m128 row2 = _mm_sub_ps(this->rows[2], rhs.rows[2]);
        const __m128 row3 = _mm_sub_ps(this->rows[3], rhs.rows[3]);
        
        Matrix4x4_SIMD resultMatrix;
        resultMatrix.rows[0] = row0;
        resultMatrix.rows[1] = row1;
        resultMatrix.rows[2] = row2;
        resultMatrix.rows[3] = row3;
        
        return resultMatrix;
    }

    Matrix4x4_SIMD Matrix4x4_SIMD::operator*(const Matrix4x4_SIMD& rhs) const
    {
        Matrix4x4_SIMD result = Zero();

        for (std::size_t row = 0; row < 4; ++row)
        {
            for (std::size_t column = 0; column < 4; ++column)
            {
                for (std::size_t k = 0; k < 4; ++k)
                {
                    result(row, column) +=
                        (*this)(row, k) * rhs(k, column);
                }
            }
        }

        return result;
    }

    Vec4_SIMD Matrix4x4_SIMD::operator*(const Vec4_SIMD& rhs) const
    {
        const __m128 vecRhs = rhs.data;

        const __m128 rowOneResult = _mm_mul_ps(rows[0], vecRhs);
        const __m128 rowTwoResult = _mm_mul_ps(rows[1], vecRhs);
        const __m128 rowThreeResult = _mm_mul_ps(rows[2], vecRhs);
        const __m128 rowFourResult = _mm_mul_ps(rows[3], vecRhs);

        const __m128 row1XYResult = _mm_hadd_ps(rowOneResult, rowOneResult);
        const __m128 row1FinalResult = _mm_hadd_ps(row1XYResult, row1XYResult);

        const __m128 row2XYResult = _mm_hadd_ps(rowTwoResult, rowTwoResult);
        const __m128 row2FinalResult = _mm_hadd_ps(row2XYResult, row2XYResult);

        const __m128 row3XYResult = _mm_hadd_ps(rowThreeResult, rowThreeResult);
        const __m128 row3FinalResult = _mm_hadd_ps(row3XYResult, row3XYResult);

        const __m128 row4XYResult = _mm_hadd_ps(rowFourResult, rowFourResult);
        const __m128 row4FinalResult = _mm_hadd_ps(row4XYResult, row4XYResult);

        const float x = _mm_cvtss_f32(row1FinalResult);
        const float y = _mm_cvtss_f32(row2FinalResult);
        const float z = _mm_cvtss_f32(row3FinalResult);
        const float w = _mm_cvtss_f32(row4FinalResult);

        return Vec4_SIMD(_mm_setr_ps(x, y, z, w));
    }

    Matrix4x4_SIMD Matrix4x4_SIMD::operator*(float scalar) const
    {
        const __m128 scalarSIMD = _mm_set1_ps(scalar);

        const __m128 rowOneResult = _mm_mul_ps(rows[0], scalarSIMD);
        const __m128 rowTwoResult = _mm_mul_ps(rows[1], scalarSIMD);
        const __m128 rowThreeResult = _mm_mul_ps(rows[2], scalarSIMD);
        const __m128 rowFourResult = _mm_mul_ps(rows[3], scalarSIMD);

        return {
            rowOneResult,
            rowTwoResult,
            rowThreeResult,
            rowFourResult
        };
    }

    Matrix4x4_SIMD& Matrix4x4_SIMD::operator*=(
        const Matrix4x4_SIMD& rhs)
    {
        Matrix4x4_SIMD result = Zero();

        for (std::size_t row = 0; row < 4; ++row)
        {
            for (std::size_t column = 0; column < 4; ++column)
            {
                for (std::size_t k = 0; k < 4; ++k)
                {
                    result(row, column) +=
                        (*this)(row, k) * rhs(k, column);
                }
            }
        }

        *this = result;
        return *this;
    }

    bool Matrix4x4_SIMD::operator==(const Matrix4x4_SIMD& rhs) const
    {
        for (std::size_t row = 0; row < 4; ++row)
        {
            const __m128 cmp = _mm_cmpeq_ps(rows[row], rhs.rows[row]);

            if (_mm_movemask_ps(cmp) != 0xF)
            {
                return false;
            }
        }

        return true;
    }

    bool Matrix4x4_SIMD::operator!=(
        const Matrix4x4_SIMD& rhs) const
    {
        return !(*this == rhs);
    }

#pragma endregion


    Matrix4x4_SIMD Matrix4x4_SIMD::Transpose() const
    {
        __m128 row0 = rows[0];
        __m128 row1 = rows[1];
        __m128 row2 = rows[2];
        __m128 row3 = rows[3];

        _MM_TRANSPOSE4_PS(row0, row1, row2, row3);

        return Matrix4x4_SIMD(row0, row1, row2, row3);
    }

    float Matrix4x4_SIMD::Determinant() const
    {
        Matrix4x4_SIMD working = *this;
        float determinant = 1.0f;

        for (std::size_t column = 0; column < 4; ++column)
        {
            std::size_t pivot = column;

            for (std::size_t row = column + 1; row < 4; ++row)
            {
                if (std::abs(working(row, column)) >
                    std::abs(working(pivot, column)))
                {
                    pivot = row;
                }
            }

            if (working(pivot, column) == 0.0f)
            {
                return 0.0f;
            }

            if (pivot != column)
            {
                std::swap(working.rows[pivot], working.rows[column]);
                determinant = -determinant;
            }

            const float diagonal = working(column, column);
            determinant *= diagonal;

            for (std::size_t row = column + 1; row < 4; ++row)
            {
                const float factor = working(row, column) / diagonal;

                const __m128 factorSIMD = _mm_set1_ps(factor);
                const __m128 multiplied = _mm_mul_ps(
                    factorSIMD,
                    working.rows[column]
                );

                working.rows[row] = _mm_sub_ps(
                    working.rows[row],
                    multiplied
                );
            }
        }

        return determinant;
    }

Matrix4x4_SIMD Matrix4x4_SIMD::Inverse(float relativeTolerance) const
{
    if (!std::isfinite(relativeTolerance) ||
        relativeTolerance < 0.0f ||
        relativeTolerance >= 1.0f)
    {
        throw std::invalid_argument(
            "Inverse tolerance must be finite and in [0, 1)"
        );
    }

    Matrix4x4_SIMD left = *this;
    Matrix4x4_SIMD right = Identity();

    float scales[4]{};

    for (std::size_t row = 0; row < 4; ++row)
    {
        for (std::size_t column = 0; column < 4; ++column)
        {
            const float value = left(row, column);

            if (!std::isfinite(value))
            {
                throw std::domain_error(
                    "Cannot invert a non-finite matrix"
                );
            }

            scales[row] = std::max(
                scales[row],
                std::abs(value)
            );
        }

        if (scales[row] == 0.0f)
        {
            throw std::domain_error(
                "Cannot invert a singular matrix"
            );
        }
    }

    for (std::size_t column = 0; column < 4; ++column)
    {
        std::size_t pivot = column;

        for (std::size_t row = column + 1; row < 4; ++row)
        {
            if (std::abs(left(row, column)) / scales[row] >
                std::abs(left(pivot, column)) / scales[pivot])
            {
                pivot = row;
            }
        }

        if (std::abs(left(pivot, column)) / scales[pivot] <=
            relativeTolerance)
        {
            throw std::domain_error(
                "Matrix is singular or too ill-conditioned"
            );
        }

        if (pivot != column)
        {
            std::swap(left.rows[column], left.rows[pivot]);
            std::swap(right.rows[column], right.rows[pivot]);
            std::swap(scales[column], scales[pivot]);
        }

        const float diagonal = left(column, column);
        const __m128 diagonalSIMD = _mm_set1_ps(diagonal);

        left.rows[column] = _mm_div_ps(
            left.rows[column],
            diagonalSIMD
        );

        right.rows[column] = _mm_div_ps(
            right.rows[column],
            diagonalSIMD
        );

        for (std::size_t row = 0; row < 4; ++row)
        {
            if (row == column)
            {
                continue;
            }

            const float factor = left(row, column);
            const __m128 factorSIMD = _mm_set1_ps(factor);

            const __m128 leftProduct = _mm_mul_ps(
                factorSIMD,
                left.rows[column]
            );

            const __m128 rightProduct = _mm_mul_ps(
                factorSIMD,
                right.rows[column]
            );

            left.rows[row] = _mm_sub_ps(
                left.rows[row],
                leftProduct
            );

            right.rows[row] = _mm_sub_ps(
                right.rows[row],
                rightProduct
            );
        }
    }

    for (std::size_t row = 0; row < 4; ++row)
    {
        for (std::size_t column = 0; column < 4; ++column)
        {
            if (!std::isfinite(right(row, column)))
            {
                throw std::domain_error(
                    "Inverse is not representable"
                );
            }
        }
    }

    return right;
}


    Matrix4x4_SIMD Matrix4x4_SIMD::Scale(const Vec4_SIMD& scale)
    {
        const __m128 scaleRegister = scale.data;

        const __m128 row1 = _mm_shuffle_ps(scaleRegister, scaleRegister, _MM_SHUFFLE(3, 3, 3, 0));
        const __m128 row2 = _mm_shuffle_ps(scaleRegister, scaleRegister, _MM_SHUFFLE(3, 3, 1, 3));
        const __m128 row3 = _mm_shuffle_ps(scaleRegister, scaleRegister, _MM_SHUFFLE(3, 2, 3, 3));
        const __m128 row4 = _mm_shuffle_ps(scaleRegister, scaleRegister, _MM_SHUFFLE(3, 3, 3, 3));

        return {
            row1,
            row2,
            row3,
            row4
        };
    }

    Matrix4x4_SIMD Matrix4x4_SIMD::RotationX(float radians)
    {
        const float c = std::cos(radians);
        const float s = std::sin(radians);

        return {
            _mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f),
            _mm_setr_ps(0.0f, c, -s, 0.0f),
            _mm_setr_ps(0.0f, s, c, 0.0f),
            _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f)
        };
    }

    Matrix4x4_SIMD Matrix4x4_SIMD::RotationY(float radians)
    {
        const float c = std::cos(radians);
        const float s = std::sin(radians);

        return {
            _mm_setr_ps(c, 0.0f, s, 0.0f),
            _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f),
            _mm_setr_ps(-s, 0.0f, c, 0.0f),
            _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f)
        };
    }

    Matrix4x4_SIMD Matrix4x4_SIMD::RotationZ(float radians)
    {
        const float c = std::cos(radians);
        const float s = std::sin(radians);

        return {
            _mm_setr_ps(c, -s, 0.0f, 0.0f),
            _mm_setr_ps(s, c, 0.0f, 0.0f),
            _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f),
            _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f)
        };
    }

    Matrix4x4_SIMD Matrix4x4_SIMD::Translation(const Vec3_SIMD& offset)
    {
        const __m128 offsetRegister = offset.Load(offset.data);

        const float x = _mm_cvtss_f32(offsetRegister);
        const float y = _mm_cvtss_f32(_mm_shuffle_ps(offsetRegister, offsetRegister, _MM_SHUFFLE(1, 1, 1, 1)));
        const float z = _mm_cvtss_f32(_mm_shuffle_ps(offsetRegister, offsetRegister, _MM_SHUFFLE(2, 2, 2, 2)));

        return {
            _mm_setr_ps(1.0f, 0.0f, 0.0f, x),
            _mm_setr_ps(0.0f, 1.0f, 0.0f, y),
            _mm_setr_ps(0.0f, 0.0f, 1.0f, z),
            _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f)
        };
    }

    Vec3_SIMD Matrix4x4_SIMD::TransformPoint(
        const Vec3_SIMD& point) const
    {
        const __m128 vecPoint = point.Load(point.data);

        const __m128 rowOneResult = _mm_mul_ps(rows[0], vecPoint);
        const __m128 rowTwoResult = _mm_mul_ps(rows[1], vecPoint);
        const __m128 rowThreeResult = _mm_mul_ps(rows[2], vecPoint);

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

    Vec3_SIMD Matrix4x4_SIMD::TransformDirection(
        const Vec3_SIMD& direction) const
    {
        const __m128 vecDirection = direction.Load(direction.data);

        const __m128 rowOneResult = _mm_mul_ps(rows[0], vecDirection);
        const __m128 rowTwoResult = _mm_mul_ps(rows[1], vecDirection);
        const __m128 rowThreeResult = _mm_mul_ps(rows[2], vecDirection);

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
}