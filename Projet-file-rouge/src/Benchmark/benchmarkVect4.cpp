#include <benchmark/benchmark.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <filesystem>
#include <random>
#include <cstdint>
#include "MathLib/Vec4.h"
#include "benchmarkUtils.h"

#define REGISTER_VEC4_BENCHMARK(func) \
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

// + 
static void BM_Vect4_SUM(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v2(count);
    std::vector<Maths::Vec4<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i] + v2[i];   
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_SUM);

// - 
static void BM_Vect4_SUB(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v2(count);
    std::vector<Maths::Vec4<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i] - v2[i];
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_SUB);

// * 
static void BM_Vect4_MUL(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v2(count);
    std::vector<Maths::Vec4<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i] * v2[i];
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_MUL);

// / 
static void BM_Vect4_DIV(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v2(count);
    std::vector<Maths::Vec4<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        // Evite la division par zero
        v2[i] = {dist(rng) + 0.1f, dist(rng) + 0.1f, dist(rng) + 0.1f, dist(rng) + 0.1f};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i] / v2[i];
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_DIV);

// Unary -
static void BM_Vect4_NEG(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = -v1[i];
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_NEG);

// * scalar 
static void BM_Vect4_MULScalar(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<float> scalars(count);
    std::vector<Maths::Vec4<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        scalars[i] = dist(rng);
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i] * scalars[i];
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_MULScalar);

// / scalar
static void BM_Vect4_DIVScalar(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<float> scalars(count);
    std::vector<Maths::Vec4<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        scalars[i] = dist(rng) + 0.1f;
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i] / scalars[i]; 
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_DIVScalar);

// += 
static void BM_Vect4_ADDAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v1_original(count);
    std::vector<Maths::Vec4<float>> v2(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }
    v1_original = v1;

    for (auto _ : state)
    {
        state.PauseTiming();
        v1 = v1_original;
        state.ResumeTiming(); 

        for (std::size_t i = 0; i < count; ++i)
        {
            v1[i] += v2[i];    
            benchmark::DoNotOptimize(v1[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_ADDAssign);

// -=
static void BM_Vect4_SUBAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v2(count);
    std::vector<Maths::Vec4<float>> v1_original(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }
    v1_original = v1;
    
    for (auto _ : state)
    {
        state.PauseTiming();
        v1 = v1_original;
        state.ResumeTiming(); 
        
        for (std::size_t i = 0; i < count; ++i)
        {
            v1[i] -= v2[i];
            benchmark::DoNotOptimize(v1[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_SUBAssign);

// *=
static void BM_Vect4_MULAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v2(count);
    std::vector<Maths::Vec4<float>> v1_original(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }
    v1_original = v1;

    for (auto _ : state)
    {
        state.PauseTiming();
        v1 = v1_original;
        state.ResumeTiming(); 
        
        for (std::size_t i = 0; i < count; ++i)
        {
            v1[i] *= v2[i];
            benchmark::DoNotOptimize(v1[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_MULAssign);

// /=
static void BM_Vect4_DIVAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v1_original(count);
    std::vector<Maths::Vec4<float>> v2(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng) + 0.1f, dist(rng) + 0.1f, dist(rng) + 0.1f, dist(rng) + 0.1f};
    }
    v1_original = v1;

    for (auto _ : state)
    {
        state.PauseTiming();
        v1 = v1_original;
        state.ResumeTiming(); 
        
        for (std::size_t i = 0; i < count; ++i)
        {
            v1[i] /= v2[i];
            benchmark::DoNotOptimize(v1[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_DIVAssign);

// *= scalar
static void BM_Vect4_MulScalarAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v1_original(count);
    std::vector<float> scalars(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        scalars[i] = dist(rng);
    }
    v1_original = v1;

    for (auto _ : state)
    {
        state.PauseTiming();
        v1 = v1_original;
        state.ResumeTiming(); 
        
        for (std::size_t i = 0; i < count; ++i)
        {
            v1[i] *= scalars[i];
            benchmark::DoNotOptimize(v1[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_MulScalarAssign);

// /= scalar
static void BM_Vect4_DIVScalarAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v1_original(count);
    std::vector<float> scalars(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        scalars[i] = dist(rng) + 0.1f;
    }
    v1_original = v1;

    for (auto _ : state)
    {
        state.PauseTiming();
        v1 = v1_original;
        state.ResumeTiming(); 
        
        for (std::size_t i = 0; i < count; ++i)
        {
            v1[i] /= scalars[i];
            benchmark::DoNotOptimize(v1[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_DIVScalarAssign);

// ==
static void BM_Vect4_Equal(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v2(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            bool output = (v1[i] == v2[i]);    
            benchmark::DoNotOptimize(output);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_Equal);

// !=
static void BM_Vect4_NotEqual(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v2(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            bool output = (v1[i] != v2[i]);    
            benchmark::DoNotOptimize(output);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_NotEqual);

// Dot
static void BM_Vect4_Dot(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v2(count);
    std::vector<float> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i].Dot(v2[i]);
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_Dot);

// MagnitudeSquared
static void BM_Vect4_MagnitudeSquared(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<float> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i].MagnitudeSquared();
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_MagnitudeSquared);

// Magnitude
static void BM_Vect4_Magnitude(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<float> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i].Magnitude();
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_Magnitude);

// Normalize
static void BM_Vect4_Normalize(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng) + 0.1f, dist(rng) + 0.1f, dist(rng) + 0.1f, dist(rng) + 0.1f};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i].Normalize();
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_Normalize);

// DistanceSquared
static void BM_Vect4_DistanceSquared(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v2(count);
    std::vector<float> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i].DistanceSquared(v2[i]);
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_DistanceSquared);

// Distance
static void BM_Vect4_Distance(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v2(count);
    std::vector<float> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i].Distance(v2[i]);
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_Distance);

// Angle
static void BM_Vect4_Angle(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v2(count);
    std::vector<float> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng) + 0.1f, dist(rng) + 0.1f, dist(rng) + 0.1f, dist(rng) + 0.1f};
        v2[i] = {dist(rng) + 0.1f, dist(rng) + 0.1f, dist(rng) + 0.1f, dist(rng) + 0.1f};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i].Angle(v2[i]);
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_Angle);

// Lerp
static void BM_Vect4_Lerp(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    std::uniform_real_distribution<float> dist_t(0.0f, 1.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v2(count);
    std::vector<float> t_factors(count);
    std::vector<Maths::Vec4<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        t_factors[i] = dist_t(rng);
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = Maths::Vec4<float>::Lerp(v1[i], v2[i], t_factors[i]);
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_Lerp);

// Min
static void BM_Vect4_Min(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v2(count);
    std::vector<Maths::Vec4<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = Maths::Vec4<float>::Min(v1[i], v2[i]);
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_Min);

// Max
static void BM_Vect4_Max(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
    
    std::vector<Maths::Vec4<float>> v1(count);
    std::vector<Maths::Vec4<float>> v2(count);
    std::vector<Maths::Vec4<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = Maths::Vec4<float>::Max(v1[i], v2[i]);
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC4_BENCHMARK(BM_Vect4_Max);