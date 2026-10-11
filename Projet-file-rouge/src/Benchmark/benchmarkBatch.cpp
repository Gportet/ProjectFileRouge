#include <benchmark/benchmark.h>
#include <vector>
#include <random>
#include <cmath>
#include <cstdint>
#include <array>

#include "MathLib_SIMD/include/Vec3_SIMD.h"
#include "MathLib_SIMD/include/Vec4_SIMD.h"
#include "MathLib_SIMD/include/Matrix3x3_SIMD.h"
#include "MathLib_SIMD/include/Matrix4x4_SIMD.h"
#include "MathLib_SIMD/include/Vec3_ASM.h"
#include "benchmarkUtils.h"

// ----------------------------------------------------------------------------
// Structures C++ scalaires (AoS) pour la comparaison
// ----------------------------------------------------------------------------
struct Vec3AoS { float x, y, z; };
struct Vec4AoS { float x, y, z, w; };

struct Matrix3x3AoS { float m[3][3]; };
struct Matrix4x4AoS { float m[4][4]; };

inline void SetCyclesCounter(benchmark::State& state, std::size_t count)
{
    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
    state.counters["Cycles/op"] = benchmark::Counter(
        static_cast<double>(state.range(0)), 
        benchmark::Counter::kIsIterationInvariantRate
    );
}

// ============================================================================
// 1. VEC3_SIMD (Dot, Cross, Normalize)
// ============================================================================

// --- Dot Product ---
static void BM_Batch_Vec3_Dot_CPP(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<Vec3AoS> v1(count), v2(count);
    std::vector<float> output(count);

    for (std::size_t i = 0; i < count; ++i) {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i] = v1[i].x * v2[i].x + v1[i].y * v2[i].y + v1[i].z * v2[i].z;
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Vec3_Dot_CPP)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_Batch_Vec3_Dot_SIMD(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<MathsSIMD::Vec3_SIMD> v1(count), v2(count);
    std::vector<float> output(count);

    for (std::size_t i = 0; i < count; ++i) {
        v1[i] = MathsSIMD::Vec3_SIMD(dist(rng), dist(rng), dist(rng));
        v2[i] = MathsSIMD::Vec3_SIMD(dist(rng), dist(rng), dist(rng));
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i] = v1[i].Dot(v2[i]);
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Vec3_Dot_SIMD)->Arg(1000)->Arg(10000)->Arg(100000);

// --- Cross Product ---
static void BM_Batch_Vec3_Cross_CPP(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<Vec3AoS> v1(count), v2(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i].x = v1[i].y * v2[i].z - v1[i].z * v2[i].y;
            output[i].y = v1[i].z * v2[i].x - v1[i].x * v2[i].z;
            output[i].z = v1[i].x * v2[i].y - v1[i].y * v2[i].x;
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Vec3_Cross_CPP)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_Batch_Vec3_Cross_SIMD(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<MathsSIMD::Vec3_SIMD> v1(count), v2(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        v1[i] = MathsSIMD::Vec3_SIMD(dist(rng), dist(rng), dist(rng));
        v2[i] = MathsSIMD::Vec3_SIMD(dist(rng), dist(rng), dist(rng));
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i] = v1[i].Cross(v2[i]);
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Vec3_Cross_SIMD)->Arg(1000)->Arg(10000)->Arg(100000);

// --- Normalize ---
static void BM_Batch_Vec3_Normalize_CPP(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<Vec3AoS> input(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        input[i] = {dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            float sqrMag = input[i].x * input[i].x + input[i].y * input[i].y + input[i].z * input[i].z;
            if (sqrMag > 1e-8f) {
                float invMag = 1.0f / std::sqrt(sqrMag);
                output[i] = {input[i].x * invMag, input[i].y * invMag, input[i].z * invMag};
            } else {
                output[i] = {0.0f, 0.0f, 0.0f};
            }
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Vec3_Normalize_CPP)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_Batch_Vec3_Normalize_SIMD(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<MathsSIMD::Vec3_SIMD> input(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        input[i] = MathsSIMD::Vec3_SIMD(dist(rng), dist(rng), dist(rng));
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i] = input[i].Normalize();
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Vec3_Normalize_SIMD)->Arg(1000)->Arg(10000)->Arg(100000);

// ============================================================================
// 2. VEC4_SIMD (Dot, Normalize, Lerp)
// ============================================================================

// --- Dot Product ---
static void BM_Batch_Vec4_Dot_CPP(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<Vec4AoS> v1(count), v2(count);
    std::vector<float> output(count);

    for (std::size_t i = 0; i < count; ++i) {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i] = v1[i].x * v2[i].x + v1[i].y * v2[i].y + v1[i].z * v2[i].z + v1[i].w * v2[i].w;
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Vec4_Dot_CPP)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_Batch_Vec4_Dot_SIMD(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<MathsSIMD::Vec4_SIMD> v1(count), v2(count);
    std::vector<float> output(count);

    for (std::size_t i = 0; i < count; ++i) {
        v1[i] = MathsSIMD::Vec4_SIMD(_mm_setr_ps(dist(rng), dist(rng), dist(rng), dist(rng)));
        v2[i] = MathsSIMD::Vec4_SIMD(_mm_setr_ps(dist(rng), dist(rng), dist(rng), dist(rng)));
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i] = v1[i].Dot(v2[i]);
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Vec4_Dot_SIMD)->Arg(1000)->Arg(10000)->Arg(100000);

// --- Normalize ---
static void BM_Batch_Vec4_Normalize_CPP(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<Vec4AoS> input(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        input[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            float sqrMag = input[i].x * input[i].x + input[i].y * input[i].y + 
                           input[i].z * input[i].z + input[i].w * input[i].w;
            if (sqrMag > 1e-8f) {
                float invMag = 1.0f / std::sqrt(sqrMag);
                output[i] = {input[i].x * invMag, input[i].y * invMag, input[i].z * invMag, input[i].w * invMag};
            } else {
                output[i] = {0.0f, 0.0f, 0.0f, 0.0f};
            }
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Vec4_Normalize_CPP)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_Batch_Vec4_Normalize_SIMD(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<MathsSIMD::Vec4_SIMD> input(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        input[i] = MathsSIMD::Vec4_SIMD(_mm_setr_ps(dist(rng), dist(rng), dist(rng), dist(rng)));
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i] = input[i].Normalize();
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Vec4_Normalize_SIMD)->Arg(1000)->Arg(10000)->Arg(100000);

// --- Lerp ---
static void BM_Batch_Vec4_Lerp_CPP(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    const float t = 0.5f;

    std::vector<Vec4AoS> v1(count), v2(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i].x = v1[i].x + t * (v2[i].x - v1[i].x);
            output[i].y = v1[i].y + t * (v2[i].y - v1[i].y);
            output[i].z = v1[i].z + t * (v2[i].z - v1[i].z);
            output[i].w = v1[i].w + t * (v2[i].w - v1[i].w);
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Vec4_Lerp_CPP)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_Batch_Vec4_Lerp_SIMD(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    const float t = 0.5f;

    std::vector<MathsSIMD::Vec4_SIMD> v1(count), v2(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        v1[i] = MathsSIMD::Vec4_SIMD(_mm_setr_ps(dist(rng), dist(rng), dist(rng), dist(rng)));
        v2[i] = MathsSIMD::Vec4_SIMD(_mm_setr_ps(dist(rng), dist(rng), dist(rng), dist(rng)));
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i] = MathsSIMD::Vec4_SIMD::Lerp(v1[i], v2[i], t);
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Vec4_Lerp_SIMD)->Arg(1000)->Arg(10000)->Arg(100000);

// ============================================================================
// 3. MATRIX3X3_SIMD (Matrix Multiplication, Matrix-Vector Mul, Transpose)
// ============================================================================

// --- Matrix Multiplication ---
static void BM_Batch_Matrix3x3_MulMatrix_CPP(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);

    std::vector<Matrix3x3AoS> m1(count), m2(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                m1[i].m[r][c] = dist(rng);
                m2[i].m[r][c] = dist(rng);
            }
        }
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            for (int r = 0; r < 3; ++r) {
                for (int c = 0; c < 3; ++c) {
                    output[i].m[r][c] = m1[i].m[r][0] * m2[i].m[0][c] +
                                       m1[i].m[r][1] * m2[i].m[1][c] +
                                       m1[i].m[r][2] * m2[i].m[2][c];
                }
            }
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Matrix3x3_MulMatrix_CPP)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_Batch_Matrix3x3_MulMatrix_SIMD(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);

    std::vector<MathsSIMD::Matrix3x3_SIMD> m1(count), m2(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        m1[i] = MathsSIMD::Matrix3x3_SIMD(std::array<float, 9>{
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
        m2[i] = MathsSIMD::Matrix3x3_SIMD(std::array<float, 9>{
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i] = m1[i] * m2[i];
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Matrix3x3_MulMatrix_SIMD)->Arg(1000)->Arg(10000)->Arg(100000);

// --- Matrix-Vector Multiplication ---
static void BM_Batch_Matrix3x3_MulVec3_CPP(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);

    std::vector<Matrix3x3AoS> mat(count);
    std::vector<Vec3AoS> vec(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        vec[i] = {dist(rng), dist(rng), dist(rng)};
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                mat[i].m[r][c] = dist(rng);
            }
        }
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i].x = mat[i].m[0][0] * vec[i].x + mat[i].m[0][1] * vec[i].y + mat[i].m[0][2] * vec[i].z;
            output[i].y = mat[i].m[1][0] * vec[i].x + mat[i].m[1][1] * vec[i].y + mat[i].m[1][2] * vec[i].z;
            output[i].z = mat[i].m[2][0] * vec[i].x + mat[i].m[2][1] * vec[i].y + mat[i].m[2][2] * vec[i].z;
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Matrix3x3_MulVec3_CPP)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_Batch_Matrix3x3_MulVec3_SIMD(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);

    std::vector<MathsSIMD::Matrix3x3_SIMD> mat(count);
    std::vector<MathsSIMD::Vec3_SIMD> vec(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        vec[i] = MathsSIMD::Vec3_SIMD(dist(rng), dist(rng), dist(rng));
        mat[i] = MathsSIMD::Matrix3x3_SIMD(std::array<float, 9>{
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i] = mat[i] * vec[i];
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Matrix3x3_MulVec3_SIMD)->Arg(1000)->Arg(10000)->Arg(100000);

// --- Transpose ---
static void BM_Batch_Matrix3x3_Transpose_CPP(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);

    std::vector<Matrix3x3AoS> input(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                input[i].m[r][c] = dist(rng);
            }
        }
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            for (int r = 0; r < 3; ++r) {
                for (int c = 0; c < 3; ++c) {
                    output[i].m[r][c] = input[i].m[c][r];
                }
            }
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Matrix3x3_Transpose_CPP)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_Batch_Matrix3x3_Transpose_SIMD(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);

    std::vector<MathsSIMD::Matrix3x3_SIMD> input(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        input[i] = MathsSIMD::Matrix3x3_SIMD(std::array<float, 9>{
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i] = input[i].Transpose();
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Matrix3x3_Transpose_SIMD)->Arg(1000)->Arg(10000)->Arg(100000);

// ============================================================================
// 4. MATRIX4X4_SIMD (Matrix Multiplication, Matrix-Vector Mul, TransformPoint)
// ============================================================================

// --- Matrix Multiplication ---
static void BM_Batch_Matrix4x4_MulMatrix_CPP(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);

    std::vector<Matrix4x4AoS> m1(count), m2(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                m1[i].m[r][c] = dist(rng);
                m2[i].m[r][c] = dist(rng);
            }
        }
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            for (int r = 0; r < 4; ++r) {
                for (int c = 0; c < 4; ++c) {
                    output[i].m[r][c] = m1[i].m[r][0] * m2[i].m[0][c] +
                                       m1[i].m[r][1] * m2[i].m[1][c] +
                                       m1[i].m[r][2] * m2[i].m[2][c] +
                                       m1[i].m[r][3] * m2[i].m[3][c];
                }
            }
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Matrix4x4_MulMatrix_CPP)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_Batch_Matrix4x4_MulMatrix_SIMD(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);

    std::vector<MathsSIMD::Matrix4x4_SIMD> m1(count), m2(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        m1[i] = MathsSIMD::Matrix4x4_SIMD(std::array<float, 16>{
            dist(rng), dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng), dist(rng)
        });
        m2[i] = MathsSIMD::Matrix4x4_SIMD(std::array<float, 16>{
            dist(rng), dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng), dist(rng)
        });
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i] = m1[i] * m2[i];
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Matrix4x4_MulMatrix_SIMD)->Arg(1000)->Arg(10000)->Arg(100000);

// --- Matrix-Vector Multiplication ---
static void BM_Batch_Matrix4x4_MulVec4_CPP(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);

    std::vector<Matrix4x4AoS> mat(count);
    std::vector<Vec4AoS> vec(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        vec[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                mat[i].m[r][c] = dist(rng);
            }
        }
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i].x = mat[i].m[0][0]*vec[i].x + mat[i].m[0][1]*vec[i].y + mat[i].m[0][2]*vec[i].z + mat[i].m[0][3]*vec[i].w;
            output[i].y = mat[i].m[1][0]*vec[i].x + mat[i].m[1][1]*vec[i].y + mat[i].m[1][2]*vec[i].z + mat[i].m[1][3]*vec[i].w;
            output[i].z = mat[i].m[2][0]*vec[i].x + mat[i].m[2][1]*vec[i].y + mat[i].m[2][2]*vec[i].z + mat[i].m[2][3]*vec[i].w;
            output[i].w = mat[i].m[3][0]*vec[i].x + mat[i].m[3][1]*vec[i].y + mat[i].m[3][2]*vec[i].z + mat[i].m[3][3]*vec[i].w;
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Matrix4x4_MulVec4_CPP)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_Batch_Matrix4x4_MulVec4_SIMD(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-10.0f, 10.0f);

    std::vector<MathsSIMD::Matrix4x4_SIMD> mat(count);
    std::vector<MathsSIMD::Vec4_SIMD> vec(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        vec[i] = MathsSIMD::Vec4_SIMD(_mm_setr_ps(dist(rng), dist(rng), dist(rng), dist(rng)));
        mat[i] = MathsSIMD::Matrix4x4_SIMD(std::array<float, 16>{
            dist(rng), dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng), dist(rng)
        });
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i] = mat[i] * vec[i];
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Matrix4x4_MulVec4_SIMD)->Arg(1000)->Arg(10000)->Arg(100000);

// --- TransformPoint ---
static void BM_Batch_Matrix4x4_TransformPoint_CPP(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<Vec3AoS> points(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        points[i] = {dist(rng), dist(rng), dist(rng)};
    }

    const float m[4][4] = {
        { 1.0f, 0.0f, 0.0f, 10.0f },
        { 0.0f, 1.0f, 0.0f, 20.0f },
        { 0.0f, 0.0f, 1.0f, 30.0f },
        { 0.0f, 0.0f, 0.0f,  1.0f }
    };

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i].x = m[0][0] * points[i].x + m[0][1] * points[i].y + m[0][2] * points[i].z + m[0][3];
            output[i].y = m[1][0] * points[i].x + m[1][1] * points[i].y + m[1][2] * points[i].z + m[1][3];
            output[i].z = m[2][0] * points[i].x + m[2][1] * points[i].y + m[2][2] * points[i].z + m[2][3];
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Matrix4x4_TransformPoint_CPP)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_Batch_Matrix4x4_TransformPoint_SIMD(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<MathsSIMD::Vec3_SIMD> points(count), output(count);

    for (std::size_t i = 0; i < count; ++i) {
        points[i] = MathsSIMD::Vec3_SIMD(dist(rng), dist(rng), dist(rng));
    }

    MathsSIMD::Matrix4x4_SIMD mat = MathsSIMD::Matrix4x4_SIMD::Translation(MathsSIMD::Vec3_SIMD(10.0f, 20.0f, 30.0f));

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            output[i] = mat.TransformPoint(points[i]);
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Matrix4x4_TransformPoint_SIMD)->Arg(1000)->Arg(10000)->Arg(100000);