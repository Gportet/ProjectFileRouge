#include <benchmark/benchmark.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <filesystem>
#include <random>
#include <cstdint>
#include "benchmarkUtils.h"
#include "MathLib_SIMD/include/Vec3_SIMD.h"

#define REGISTER_VEC3_BENCHMARK(func) \
    BENCHMARK(func) \
        ->Arg(ITERATIONS) \
        ->Repetitions(REP) \
        ->DisplayAggregatesOnly(false) \
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

static void BM_Vect3_SUM(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    std::vector<MathsSIMD::Vec3_SIMD> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_SUM);

static void BM_Vect3_SUB(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    std::vector<MathsSIMD::Vec3_SIMD> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_SUB);

static void BM_Vect3_MUL(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    std::vector<MathsSIMD::Vec3_SIMD> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_MUL);

static void BM_Vect3_DIV(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    std::vector<MathsSIMD::Vec3_SIMD> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_DIV);

static void BM_Vect3_NEG(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_NEG);

static void BM_Vect3_MULScalar(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<float> scalars(count);
    std::vector<MathsSIMD::Vec3_SIMD> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_MULScalar);

static void BM_Vect3_DIVScalar(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<float> scalars(count);
    std::vector<MathsSIMD::Vec3_SIMD> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        scalars[i] = dist(rng);
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_DIVScalar);

static void BM_Vect3_ADDAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v1_original(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_ADDAssign);

static void BM_Vect3_SUBAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    std::vector<MathsSIMD::Vec3_SIMD> v1_original(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_SUBAssign);

static void BM_Vect3_MULAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    std::vector<MathsSIMD::Vec3_SIMD> v1_original(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_MULAssign);

static void BM_Vect3_DIVAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v1_original(count);
    std::vector<float> scalars(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
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
            v1[i] /= scalars[i];
            benchmark::DoNotOptimize(v1[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC3_BENCHMARK(BM_Vect3_DIVAssign);

static void BM_Vect3_MulScalarAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v1_original(count);
    std::vector<float> scalars(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_MulScalarAssign);

static void BM_Vect3_DIVScalarAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v1_original(count);
    std::vector<float> scalars(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
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
            v1[i] /= scalars[i];
            benchmark::DoNotOptimize(v1[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC3_BENCHMARK(BM_Vect3_DIVScalarAssign);

static void BM_Vect3_Equal(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_Equal);

static void BM_Vect3_NotEqual(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_NotEqual);

static void BM_Vect3_Dot(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    std::vector<float> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_Dot);

static void BM_Vect3_Cross(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    std::vector<MathsSIMD::Vec3_SIMD> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i].Cross(v2[i]);
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC3_BENCHMARK(BM_Vect3_Cross);

static void BM_Vect3_MagnitudeSquared(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<float> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_MagnitudeSquared);

static void BM_Vect3_Magnitude(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<float> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_Magnitude);

static void BM_Vect3_Normalize(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_Normalize);

static void BM_Vect3_DistanceSquared(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    std::vector<float> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_DistanceSquared);

static void BM_Vect3_Distance(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    std::vector<float> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_Distance);

static void BM_Vect3_Angle(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    std::vector<float> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
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
REGISTER_VEC3_BENCHMARK(BM_Vect3_Angle);

static void BM_Vect3_Lerp(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    std::uniform_real_distribution<float> dist_t(0.0f, 1.0f);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    std::vector<float> t_factors(count);
    std::vector<MathsSIMD::Vec3_SIMD> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
        t_factors[i] = dist_t(rng);
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = MathsSIMD::Vec3_SIMD::Lerp(v1[i], v2[i], t_factors[i]);
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC3_BENCHMARK(BM_Vect3_Lerp);

static void BM_Vect3_Min(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    std::vector<MathsSIMD::Vec3_SIMD> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = MathsSIMD::Vec3_SIMD::Min(v1[i], v2[i]);
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC3_BENCHMARK(BM_Vect3_Min);

static void BM_Vect3_Max(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    std::vector<MathsSIMD::Vec3_SIMD> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = MathsSIMD::Vec3_SIMD::Max(v1[i], v2[i]);
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
REGISTER_VEC3_BENCHMARK(BM_Vect3_Max);