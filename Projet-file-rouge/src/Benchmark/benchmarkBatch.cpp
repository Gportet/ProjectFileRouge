#include <benchmark/benchmark.h>
#include <vector>
#include <random>
#include <cmath>
#include <cstdint>

#include "MathLib_SIMD/include/Vec3_SIMD.h"
#include "MathLib_SIMD/include/Matrix4x4_SIMD.h"
#include "MathLib_SIMD/include/Vec3_ASM.h"
#include "benchmarkUtils.h"

struct Vec3AoS {
    float x, y, z;
};

inline void SetCyclesCounter(benchmark::State& state, std::size_t count)
{
    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
    state.counters["Cycles/op"] = benchmark::Counter(
        static_cast<double>(state.range(0)), 
        benchmark::Counter::kIsIterationInvariantRate
    );
}

// ============================================================================
// 1. PRODUIT SCALAIRE (DOT PRODUCT)
// ============================================================================

static void BM_Batch_Dot_CPP(benchmark::State& state) {
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
BENCHMARK(BM_Batch_Dot_CPP)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_Batch_Dot_SIMD(benchmark::State& state) {
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
BENCHMARK(BM_Batch_Dot_SIMD)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_Batch_Dot_ASM(benchmark::State& state) {
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
            output[i] = v1[i].DotASM(v2[i]);
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Dot_ASM)->Arg(1000)->Arg(10000)->Arg(100000);

// ============================================================================
// 2. NORMALISATION DE VECTEURS (NORMALIZE)
// ============================================================================

static void BM_Batch_Normalize_CPP(benchmark::State& state) {
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
BENCHMARK(BM_Batch_Normalize_CPP)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_Batch_Normalize_SIMD(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<MathsSIMD::Vec3_SIMD> input(count);
    std::vector<MathsSIMD::Vec3_SIMD> output(count);

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
BENCHMARK(BM_Batch_Normalize_SIMD)->Arg(1000)->Arg(10000)->Arg(100000);

// ============================================================================
// 3. TRANSFORMATION DE POINTS (TRANSFORM POINT 3D)
// ============================================================================

static void BM_Batch_TransformPoint_CPP(benchmark::State& state) {
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
BENCHMARK(BM_Batch_TransformPoint_CPP)->Arg(1000)->Arg(10000)->Arg(100000);

static void BM_Batch_TransformPoint_SIMD(benchmark::State& state) {
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<MathsSIMD::Vec3_SIMD> points(count);
    std::vector<MathsSIMD::Vec3_SIMD> output(count);

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
BENCHMARK(BM_Batch_TransformPoint_SIMD)->Arg(1000)->Arg(10000)->Arg(100000);