#include <benchmark/benchmark.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <intrin.h> // Pour __rdtsc() sous Windows/MSVC

#include "MathLib/Vec4.h"

// Calcul sur-mesure du Percentile 95 (P95)
double ComputeP95(const std::vector<double>& samples)
{
    std::vector<double> sorted = samples;
    std::sort(sorted.begin(), sorted.end());
    const auto index = static_cast<std::size_t>(std::ceil(0.95 * sorted.size())) - 1;
    return sorted[index];
}

static void BM_Vec4Addition(benchmark::State& state)
{
    const std::size_t count = state.range(0);

    std::vector<Maths::Vec4<float>> left(count, {1.0f, 2.0f, 3.0f, 4.0f});
    std::vector<Maths::Vec4<float>> right(count, {5.0f, 6.0f, 7.0f, 8.0f});
    std::vector<Maths::Vec4<float>> output(count);

    const unsigned __int64 start_cycles = __rdtsc();

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            output[i] = left[i] + right[i];
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }

    const unsigned __int64 end_cycles = __rdtsc();

    // Nombre d'opérations exécutées (utilisé pour déduire le temps/op)
    const double total_ops = static_cast<double>(count) * state.iterations();
    state.SetItemsProcessed(static_cast<int64_t>(total_ops));

    // Cycles par opération (via __rdtsc)
    const double total_cycles = static_cast<double>(end_cycles - start_cycles);
    state.counters["Cycles/op"] = total_cycles / total_ops;
}

BENCHMARK(BM_Vec4Addition)
    ->Arg(250000)
    ->Repetitions(10)                       // Taux / Marge d'erreur (StdDev/CV%)
    ->DisplayAggregatesOnly(false)
    ->ComputeStatistics("p95", ComputeP95); // Percentile 95

// Génère automatiquement la fonction main() de Google Benchmark
BENCHMARK_MAIN();