#include <benchmark/benchmark.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <filesystem>
#include <random>
#include <cstdint>
#include "MathLib/Vec3.h"
#include "benchmarkUtils.h"

// basic operators

// + 
static void BM_Vect3_SUM(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
    std::vector<Maths::Vec3<float>> output(count);
    
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_SUM)
    ->Arg(ITERATIONS)                             // Taille du jeu de données 
    ->Repetitions(REP)                         // Calcul de la marge d'erreur (StdDev / CV%)
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// - 
static void BM_Vect3_SUB(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
    std::vector<Maths::Vec3<float>> output(count);
    
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_SUB)
    ->Arg(ITERATIONS)                               
    ->Repetitions(REP)                         
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// * 
static void BM_Vect3_MUL(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
    std::vector<Maths::Vec3<float>> output(count);
    
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_MUL)
    ->Arg(ITERATIONS)                               
    ->Repetitions(REP)                         
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// / 
static void BM_Vect3_DIV(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
    std::vector<Maths::Vec3<float>> output(count);
    
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_DIV)
    ->Arg(ITERATIONS)                               
    ->Repetitions(REP)                         
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// negative 
static void BM_Vect3_NEG(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> output(count);
    
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_NEG)
    ->Arg(ITERATIONS)                               
    ->Repetitions(REP)                         
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// * scalar 
static void BM_Vect3_MULScalar(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<float> scalars(count);
    std::vector<Maths::Vec3<float>> output(count);
    
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_MULScalar)
    ->Arg(ITERATIONS)                               
    ->Repetitions(REP)                         
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// / Scalar
static void BM_Vect3_DIVScalar(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<float> scalars(count);
    std::vector<Maths::Vec3<float>> output(count);
    
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_DIVScalar)
    ->Arg(ITERATIONS)                               
    ->Repetitions(REP)                         
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// += 
static void BM_Vect3_ADDAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v1_original(count);
    std::vector<Maths::Vec3<float>> v2(count);
    
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_ADDAssign)
    ->Arg(ITERATIONS)                             
    ->Repetitions(REP)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// -=
static void BM_Vect3_SUBAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
    std::vector<Maths::Vec3<float>> v1_original(count);
    
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_SUBAssign)
    ->Arg(ITERATIONS)                             
    ->Repetitions(REP)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// *=
static void BM_Vect3_MULAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
    std::vector<Maths::Vec3<float>> v1_original(count);
    
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_MULAssign)
    ->Arg(ITERATIONS)                             
    ->Repetitions(REP)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// /=
static void BM_Vect3_DIVAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v1_original(count);
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_DIVAssign)
    ->Arg(ITERATIONS)                               
    ->Repetitions(REP)                         
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// *= scalar
static void BM_Vect3_MulScalarAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v1_original(count);
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_MulScalarAssign)
    ->Arg(ITERATIONS)                               
    ->Repetitions(REP)                         
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// /= scalar
static void BM_Vect3_DIVScalarAssign(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v1_original(count);
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_DIVScalarAssign)
    ->Arg(ITERATIONS)                               
    ->Repetitions(REP)                         
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// ==
static void BM_Vect3_Equal(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
    
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_Equal)
    ->Arg(ITERATIONS)                             
    ->Repetitions(REP)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// !=
static void BM_Vect3_NotEqual(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
    
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_NotEqual)
    ->Arg(ITERATIONS)                             
    ->Repetitions(REP)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// Dot
static void BM_Vect3_Dot(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_Dot)
    ->Arg(ITERATIONS)                             
    ->Repetitions(REP)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// Cross
static void BM_Vect3_Cross(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
    std::vector<Maths::Vec3<float>> output(count);
    
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_Cross)
    ->Arg(ITERATIONS)                             
    ->Repetitions(REP)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// MagnitudeSquared
static void BM_Vect3_MagnitudeSquared(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_MagnitudeSquared)
    ->Arg(ITERATIONS)                             
    ->Repetitions(REP)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// Magnitude
static void BM_Vect3_Magnitude(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_Magnitude)
    ->Arg(ITERATIONS)                             
    ->Repetitions(REP)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// Normalize
static void BM_Vect3_Normalize(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> output(count);
    
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_Normalize)
    ->Arg(ITERATIONS)                             
    ->Repetitions(REP)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// DistanceSquared
static void BM_Vect3_DistanceSquared(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_DistanceSquared)
    ->Arg(ITERATIONS)                             
    ->Repetitions(REP)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// Distance
static void BM_Vect3_Distance(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_Distance)
    ->Arg(ITERATIONS)                             
    ->Repetitions(REP)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// Angle
static void BM_Vect3_Angle(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
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

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_Angle)
    ->Arg(ITERATIONS)                             
    ->Repetitions(REP)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// Lerp
static void BM_Vect3_Lerp(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    std::uniform_real_distribution<float> dist_t(0.0f, 1.0f);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
    std::vector<float> t_factors(count);
    std::vector<Maths::Vec3<float>> output(count);
    
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
            output[i] = Maths::Vec3<float>::Lerp(v1[i], v2[i], t_factors[i]);
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_Lerp)
    ->Arg(ITERATIONS)                             
    ->Repetitions(REP)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// Min
static void BM_Vect3_Min(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
    std::vector<Maths::Vec3<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = Maths::Vec3<float>::Min(v1[i], v2[i]);
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_Min)
    ->Arg(ITERATIONS)                             
    ->Repetitions(REP)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

// Max
static void BM_Vect3_Max(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0, 100.0);
    
    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
    std::vector<Maths::Vec3<float>> output(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = Maths::Vec3<float>::Max(v1[i], v2[i]);
            benchmark::DoNotOptimize(output[i]);
        }
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
}
BENCHMARK(BM_Vect3_Max)
    ->Arg(ITERATIONS)                             
    ->Repetitions(REP)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);