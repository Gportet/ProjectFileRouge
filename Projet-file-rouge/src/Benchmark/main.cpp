#include <benchmark/benchmark.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <filesystem>
#include <random>
#include <cstdint>
#if defined(_MSC_VER)
    #include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
    #include <x86intrin.h>
#endif
#include "MathLib/Vec3.h"
#include "MathLib/Vec4.h"
#include "MathLib/Matrix3x3.h"
#include "MathLib/Matrix4x4.h"


int64_t ITERATIONS = 100;
// Calcul personnalisé du Percentile 95
double ComputeP95(const std::vector<double>& samples)
{
    if (samples.empty()) return 0.0;
    std::vector<double> sorted = samples;
    std::sort(sorted.begin(), sorted.end());
    const auto index = static_cast<std::size_t>(std::ceil(0.95 * static_cast<double>(sorted.size()))) - 1;
    return sorted[index];
}


# pragma region Vect3 
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
    
    const std::uint64_t start_cycles = __rdtsc();
    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i] + v2[i];    
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    const std::uint64_t end_cycles = __rdtsc();
    // Métrique 1 : Nombre total d'opérations réalisées
    const double total_ops = static_cast<double>(count) * state.iterations();
    state.SetItemsProcessed(static_cast<int64_t>(total_ops));

    // Métrique 2 : Cycles par opération
    const double total_cycles = static_cast<double>(end_cycles - start_cycles);
    state.counters["Cycles/op"] = total_cycles / total_ops;
}
BENCHMARK(BM_Vect3_SUM)
    ->Arg(ITERATIONS)                             // Taille du jeu de données 
    ->Repetitions(10)                         // Calcul de la marge d'erreur (StdDev / CV%)
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
    
    const std::uint64_t start_cycles = __rdtsc();
    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i] - v2[i];    
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    const std::uint64_t end_cycles = __rdtsc();

    const double total_ops = static_cast<double>(count) * state.iterations();
    state.SetItemsProcessed(static_cast<int64_t>(total_ops));
    
    const double total_cycles = static_cast<double>(end_cycles - start_cycles);
    state.counters["Cycles/op"] = total_cycles / total_ops;
}
BENCHMARK(BM_Vect3_SUB)
    ->Arg(ITERATIONS)                               
    ->Repetitions(10)                         
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
    
    const std::uint64_t start_cycles = __rdtsc();
    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i] * v2[i];    
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    const std::uint64_t end_cycles = __rdtsc();

    const double total_ops = static_cast<double>(count) * state.iterations();
    state.SetItemsProcessed(static_cast<int64_t>(total_ops));


    const double total_cycles = static_cast<double>(end_cycles - start_cycles);
    state.counters["Cycles/op"] = total_cycles / total_ops;
}
BENCHMARK(BM_Vect3_MUL)
    ->Arg(ITERATIONS)                               
    ->Repetitions(10)                         
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
    
    const std::uint64_t start_cycles = __rdtsc();
    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i] / v2[i];    
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    const std::uint64_t end_cycles = __rdtsc();

    const double total_ops = static_cast<double>(count) * state.iterations();
    state.SetItemsProcessed(static_cast<int64_t>(total_ops));


    const double total_cycles = static_cast<double>(end_cycles - start_cycles);
    state.counters["Cycles/op"] = total_cycles / total_ops;
}
BENCHMARK(BM_Vect3_DIV)
    ->Arg(ITERATIONS)                               
    ->Repetitions(10)                         
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
    
    const std::uint64_t start_cycles = __rdtsc();
    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = -v1[i];  
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    const std::uint64_t end_cycles = __rdtsc();

    const double total_ops = static_cast<double>(count) * state.iterations();
    state.SetItemsProcessed(static_cast<int64_t>(total_ops));


    const double total_cycles = static_cast<double>(end_cycles - start_cycles);
    state.counters["Cycles/op"] = total_cycles / total_ops;
}
BENCHMARK(BM_Vect3_NEG)
    ->Arg(ITERATIONS)                               
    ->Repetitions(10)                         
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
    
    const std::uint64_t start_cycles = __rdtsc();
    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i] * scalars[i]; 
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    const std::uint64_t end_cycles = __rdtsc();

    const double total_ops = static_cast<double>(count) * state.iterations();
    state.SetItemsProcessed(static_cast<int64_t>(total_ops));


    const double total_cycles = static_cast<double>(end_cycles - start_cycles);
    state.counters["Cycles/op"] = total_cycles / total_ops;
}
BENCHMARK(BM_Vect3_MULScalar)
    ->Arg(ITERATIONS)                               
    ->Repetitions(10)                         
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
    
    const std::uint64_t start_cycles = __rdtsc();
    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = v1[i] / scalars[i]; 
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    const std::uint64_t end_cycles = __rdtsc();

    const double total_ops = static_cast<double>(count) * state.iterations();
    state.SetItemsProcessed(static_cast<int64_t>(total_ops));


    const double total_cycles = static_cast<double>(end_cycles - start_cycles);
    state.counters["Cycles/op"] = total_cycles / total_ops;
}
BENCHMARK(BM_Vect3_DIVScalar)
    ->Arg(ITERATIONS)                               
    ->Repetitions(10)                         
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
    std::vector<Maths::Vec3<float>> v2(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
    }
    
    const std::uint64_t start_cycles = __rdtsc();
    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            v1[i] += v2[i];    
        }
        benchmark::DoNotOptimize(v1.data());
        benchmark::ClobberMemory();
    }
    const std::uint64_t end_cycles = __rdtsc();

    const double total_ops = static_cast<double>(count) * state.iterations();
    state.SetItemsProcessed(static_cast<int64_t>(total_ops));


    const double total_cycles = static_cast<double>(end_cycles - start_cycles);
    state.counters["Cycles/op"] = total_cycles / total_ops;
}
BENCHMARK(BM_Vect3_ADDAssign)
    ->Arg(ITERATIONS)                             
    ->Repetitions(10)                        
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
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
    }
    
    const std::uint64_t start_cycles = __rdtsc();
    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            v1[i] -= v2[i];    
        }
        benchmark::DoNotOptimize(v1.data());
        benchmark::ClobberMemory();
    }
    const std::uint64_t end_cycles = __rdtsc();

    const double total_ops = static_cast<double>(count) * state.iterations();
    state.SetItemsProcessed(static_cast<int64_t>(total_ops));


    const double total_cycles = static_cast<double>(end_cycles - start_cycles);
    state.counters["Cycles/op"] = total_cycles / total_ops;
}
BENCHMARK(BM_Vect3_SUBAssign)
    ->Arg(ITERATIONS)                             
    ->Repetitions(10)                        
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
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
    }
    
    const std::uint64_t start_cycles = __rdtsc();
    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            v1[i] *= v2[i];    
        }
        benchmark::DoNotOptimize(v1.data());
        benchmark::ClobberMemory();
    }
    const std::uint64_t end_cycles = __rdtsc();

    const double total_ops = static_cast<double>(count) * state.iterations();
    state.SetItemsProcessed(static_cast<int64_t>(total_ops));


    const double total_cycles = static_cast<double>(end_cycles - start_cycles);
    state.counters["Cycles/op"] = total_cycles / total_ops;
}
BENCHMARK(BM_Vect3_MULAssign)
    ->Arg(ITERATIONS)                             
    ->Repetitions(10)                        
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
    std::vector<float> scalars(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        scalars[i] = dist(rng);
    }
    
    const std::uint64_t start_cycles = __rdtsc();
    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            v1[i] /= scalars[i]; 
        }
        benchmark::DoNotOptimize(v1.data());
        benchmark::ClobberMemory();
    }
    const std::uint64_t end_cycles = __rdtsc();

    const double total_ops = static_cast<double>(count) * state.iterations();
    state.SetItemsProcessed(static_cast<int64_t>(total_ops));


    const double total_cycles = static_cast<double>(end_cycles - start_cycles);
    state.counters["Cycles/op"] = total_cycles / total_ops;
}
BENCHMARK(BM_Vect3_DIVAssign)
    ->Arg(ITERATIONS)                               
    ->Repetitions(10)                         
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
    std::vector<float> scalars(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        scalars[i] = dist(rng);
    }
    
    const std::uint64_t start_cycles = __rdtsc();
    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            v1[i] *= scalars[i]; 
        }
        benchmark::DoNotOptimize(v1.data());
        benchmark::ClobberMemory();
    }
    const std::uint64_t end_cycles = __rdtsc();

    const double total_ops = static_cast<double>(count) * state.iterations();
    state.SetItemsProcessed(static_cast<int64_t>(total_ops));


    const double total_cycles = static_cast<double>(end_cycles - start_cycles);
    state.counters["Cycles/op"] = total_cycles / total_ops;
}
BENCHMARK(BM_Vect3_MulScalarAssign)
    ->Arg(ITERATIONS)                               
    ->Repetitions(10)                         
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
    std::vector<float> scalars(count);
    
    for (std::size_t i = 0; i < count; ++i)
    {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        scalars[i] = dist(rng);
    }
    
    const std::uint64_t start_cycles = __rdtsc();
    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            v1[i] /= scalars[i]; 
        }
        benchmark::DoNotOptimize(v1.data());
        benchmark::ClobberMemory();
    }
    const std::uint64_t end_cycles = __rdtsc();

    const double total_ops = static_cast<double>(count) * state.iterations();
    state.SetItemsProcessed(static_cast<int64_t>(total_ops));


    const double total_cycles = static_cast<double>(end_cycles - start_cycles);
    state.counters["Cycles/op"] = total_cycles / total_ops;
}
BENCHMARK(BM_Vect3_DIVScalarAssign)
    ->Arg(ITERATIONS)                               
    ->Repetitions(10)                         
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
    
    const std::uint64_t start_cycles = __rdtsc();
    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            bool output = (v1[i] == v2[i]);    
            benchmark::DoNotOptimize(output);
        }
        benchmark::ClobberMemory();
    }
    const std::uint64_t end_cycles = __rdtsc();

    const double total_ops = static_cast<double>(count) * state.iterations();
    state.SetItemsProcessed(static_cast<int64_t>(total_ops));


    const double total_cycles = static_cast<double>(end_cycles - start_cycles);
    state.counters["Cycles/op"] = total_cycles / total_ops;
}
BENCHMARK(BM_Vect3_Equal)
    ->Arg(ITERATIONS)                             
    ->Repetitions(10)                        
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
    
    const std::uint64_t start_cycles = __rdtsc();
    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            bool output = (v1[i] != v2[i]);    
            benchmark::DoNotOptimize(output);
        }
        benchmark::ClobberMemory();
    }
    const std::uint64_t end_cycles = __rdtsc();

    const double total_ops = static_cast<double>(count) * state.iterations();
    state.SetItemsProcessed(static_cast<int64_t>(total_ops));


    const double total_cycles = static_cast<double>(end_cycles - start_cycles);
    state.counters["Cycles/op"] = total_cycles / total_ops;
}
BENCHMARK(BM_Vect3_NotEqual)
    ->Arg(ITERATIONS)                             
    ->Repetitions(10)                        
    ->DisplayAggregatesOnly(false)      
    ->ComputeStatistics("p95", ComputeP95)
    ->MinWarmUpTime(0.5);

# pragma endregion Vect3


# pragma region Vect4
# pragma endregion Vect4


# pragma region Matrice3
# pragma endregion Matrice3


# pragma region Matrice4
# pragma endregion Matrice4



int main(int argc, char* argv[])
{
    std::cout << "Dossier d'execution : " << std::filesystem::current_path() << std::endl;

    std::vector<char*> args(argv, argv + argc);

    char arg_out[] = "--benchmark_out=results.json";
    char arg_format[] = "--benchmark_out_format=json";

    args.push_back(arg_out);
    args.push_back(arg_format);

    int new_argc = static_cast<int>(args.size());
    char** new_argv = args.data();

    ::benchmark::Initialize(&new_argc, new_argv);
    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();

    return 0;
}