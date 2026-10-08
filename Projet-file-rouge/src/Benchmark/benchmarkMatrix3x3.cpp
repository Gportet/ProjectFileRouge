#include <benchmark/benchmark.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <filesystem>
#include <random>
#include <cstdint>
#include "MathLib/Matrix3x3.h"
#include "MathLib/Vec3.h"
#include "benchmarkUtils.h"

#define REGISTER_MAT3_BENCHMARK(func) \
    BENCHMARK(func) \
        ->Arg(ITERATIONS) \
        ->Repetitions(REP) \
        ->ReportAggregatesOnly(true) \
        ->ComputeStatistics("p95", ComputeP95) \
        ->MinWarmUpTime(0.5)

inline void SetCyclesCounter(benchmark::State& state, std::size_t count)
{
    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
    state.counters["Cycles/op"] = benchmark::Counter(
        static_cast<double>(state.range(0)), 
        benchmark::Counter::kIsIterationInvariantRate
    );
}

static void BM_Matrix3x3_ADD(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Matrix3x3<float>> m1(count);
    std::vector<Maths::Matrix3x3<float>> m2(count);
    std::vector<Maths::Matrix3x3<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        m1[i] = Maths::Matrix3x3<float>({
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
        m2[i] = Maths::Matrix3x3<float>({
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = m1[i] + m2[i];   
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_MAT3_BENCHMARK(BM_Matrix3x3_ADD);

static void BM_Matrix3x3_SUB(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Matrix3x3<float>> m1(count);
    std::vector<Maths::Matrix3x3<float>> m2(count);
    std::vector<Maths::Matrix3x3<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        m1[i] = Maths::Matrix3x3<float>({
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
        m2[i] = Maths::Matrix3x3<float>({
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = m1[i] - m2[i];   
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_MAT3_BENCHMARK(BM_Matrix3x3_SUB);

static void BM_Matrix3x3_MULMatrix(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Matrix3x3<float>> m1(count);
    std::vector<Maths::Matrix3x3<float>> m2(count);
    std::vector<Maths::Matrix3x3<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        m1[i] = Maths::Matrix3x3<float>({
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
        m2[i] = Maths::Matrix3x3<float>({
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = m1[i] * m2[i];   
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_MAT3_BENCHMARK(BM_Matrix3x3_MULMatrix);

static void BM_Matrix3x3_MULVec3(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Matrix3x3<float>> m1(count);
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        m1[i] = Maths::Matrix3x3<float>({
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
        v1[i] = {dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = m1[i] * v1[i];   
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_MAT3_BENCHMARK(BM_Matrix3x3_MULVec3);

static void BM_Matrix3x3_MULScalar(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Matrix3x3<float>> m1(count);
    std::vector<float> scalars(count);
    std::vector<Maths::Matrix3x3<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        m1[i] = Maths::Matrix3x3<float>({
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
        scalars[i] = dist(rng);
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = m1[i] * scalars[i];   
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_MAT3_BENCHMARK(BM_Matrix3x3_MULScalar);

static void BM_Matrix3x3_MULAssignMatrix(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Matrix3x3<float>> m1(count);
    std::vector<Maths::Matrix3x3<float>> m1_original(count);
    std::vector<Maths::Matrix3x3<float>> m2(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        m1[i] = Maths::Matrix3x3<float>({
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
        m2[i] = Maths::Matrix3x3<float>({
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
    }
    m1_original = m1;

    for (auto _ : state)
    {
        state.PauseTiming();
        m1 = m1_original;
        state.ResumeTiming(); 

        for (std::size_t i = 0; i < count; ++i)
        {
            m1[i] *= m2[i];   
            benchmark::DoNotOptimize(m1[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_MAT3_BENCHMARK(BM_Matrix3x3_MULAssignMatrix);

static void BM_Matrix3x3_Equal(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Matrix3x3<float>> m1(count);
    std::vector<Maths::Matrix3x3<float>> m2(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        m1[i] = Maths::Matrix3x3<float>({
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
        m2[i] = Maths::Matrix3x3<float>({
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            bool output = (m1[i] == m2[i]);   
            benchmark::DoNotOptimize(output);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_MAT3_BENCHMARK(BM_Matrix3x3_Equal);

static void BM_Matrix3x3_Transpose(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Matrix3x3<float>> m1(count);
    std::vector<Maths::Matrix3x3<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        m1[i] = Maths::Matrix3x3<float>({
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = m1[i].Transpose();   
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_MAT3_BENCHMARK(BM_Matrix3x3_Transpose);

static void BM_Matrix3x3_Determinant(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Matrix3x3<float>> m1(count);
    std::vector<float> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        m1[i] = Maths::Matrix3x3<float>({
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng), dist(rng)
        });
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = m1[i].Determinant();   
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_MAT3_BENCHMARK(BM_Matrix3x3_Determinant);

static void BM_Matrix3x3_Inverse(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(1.0f, 10.0f); // Valeurs évitant les matrices singulières
    
    std::vector<Maths::Matrix3x3<float>> m1(count);
    std::vector<Maths::Matrix3x3<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        m1[i] = Maths::Matrix3x3<float>({
            dist(rng), dist(rng), dist(rng),
            dist(rng), dist(rng) + 20.0f, dist(rng),
            dist(rng), dist(rng), dist(rng) + 40.0f
        });
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = m1[i].Inverse();   
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_MAT3_BENCHMARK(BM_Matrix3x3_Inverse);

static void BM_Matrix3x3_Scale(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec3<float>> scales(count);
    std::vector<Maths::Matrix3x3<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        scales[i] = {dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = Maths::Matrix3x3<float>::Scale(scales[i]);   
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_MAT3_BENCHMARK(BM_Matrix3x3_Scale);

static void BM_Matrix3x3_RotationX(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-3.14159f, 3.14159f);
    
    std::vector<float> angles(count);
    std::vector<Maths::Matrix3x3<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        angles[i] = dist(rng);
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = Maths::Matrix3x3<float>::RotationX(angles[i]);   
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_MAT3_BENCHMARK(BM_Matrix3x3_RotationX);