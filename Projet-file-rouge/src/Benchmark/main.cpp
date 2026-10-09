#include <benchmark/benchmark.h>
#include <vector>
#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <random>

#if defined(_MSC_VER)
    #include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
    #include <x86intrin.h>
#endif

#include "MathLib/Vec3.h"
#include "MathLib/Matrix4x4.h"
#include "MathLib/MathLib_SIMD/include/Vec3_SIMD.h"
#include "benchmarkUtils.h"

inline void SetCyclesCounter(benchmark::State& state, std::size_t count)
{
    state.SetItemsProcessed(static_cast<int64_t>(count) * state.iterations());
    state.counters["Cycles/op"] = benchmark::Counter(
        static_cast<double>(state.range(0)), 
        benchmark::Counter::kIsIterationInvariantRate
    );
}



static void BM_Batch_Dot_CPP(benchmark::State& state)
{
    const std::size_t count = state.range(0); // Taille du batch (1000, 10000, 100000)
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<Maths::Vec3<float>> v1(count);
    std::vector<Maths::Vec3<float>> v2(count);
    std::vector<float> output(count);

    for (std::size_t i = 0; i < count; ++i) {
        v1[i] = {dist(rng), dist(rng), dist(rng)};
        v2[i] = {dist(rng), dist(rng), dist(rng)};
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i) {
            output[i] = v1[i].Dot(v2[i]);
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Dot_CPP)
    ->Arg(1000)
    ->Arg(10000)
    ->Arg(100000)
    ->ReportAggregatesOnly(true);

// --- Version SIMD Batch (Chaque élément utilise Vec3_SIMD) ---
static void BM_Batch_Dot_SIMD(benchmark::State& state)
{
    const std::size_t count = state.range(0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

    std::vector<MathsSIMD::Vec3_SIMD> v1(count);
    std::vector<MathsSIMD::Vec3_SIMD> v2(count);
    std::vector<float> output(count);

    for (std::size_t i = 0; i < count; ++i) {
        v1[i] = MathsSIMD::Vec3_SIMD(dist(rng), dist(rng), dist(rng));
        v2[i] = MathsSIMD::Vec3_SIMD(dist(rng), dist(rng), dist(rng));
    }

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < count; ++i) {
            output[i] = v1[i].Dot(v2[i]);
        }
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }

    SetCyclesCounter(state, count);
}
BENCHMARK(BM_Batch_Dot_SIMD)
    ->Arg(1000)
    ->Arg(10000)
    ->Arg(100000)
    ->ReportAggregatesOnly(true);



class CustomConsoleReporter : public benchmark::ConsoleReporter {
private:
    std::string current_category = "";
    std::string current_func_base = "";

    std::string ExtractCategory(const std::string& name) const {
        if (name.find("Batch") != std::string::npos) return "BATCH SIMD";
        if (name.find("Vect3") != std::string::npos) return "VECT3";
        if (name.find("Vect4") != std::string::npos) return "VECT4";
        if (name.find("Matrix3x3") != std::string::npos) return "MATRIX 3x3";
        if (name.find("Matrix4x4") != std::string::npos) return "MATRIX 4x4";
        return "GENERAL";
    }

    std::string ExtractFuncBase(const std::string& name) const {
        size_t pos = name.find('/');
        return (pos != std::string::npos) ? name.substr(0, pos) : name;
    }

public:
    virtual void ReportRuns(const std::vector<Run>& reports) override {
        if (reports.empty()) return;

        std::string name = reports[0].benchmark_name();
        std::string category = ExtractCategory(name);
        std::string func_base = ExtractFuncBase(name);

        if (category != current_category) {
            current_category = category;
            GetOutputStream() << "\n====================================================================================================\n"
                              << "                                  === " << category << " BENCHMARKS ===\n"
                              << "====================================================================================================\n\n";
        }
        else if (func_base != current_func_base && !current_func_base.empty()) {
            GetOutputStream() << "\n----------------------------------------------------------------------------------------------------\n";
        }

        current_func_base = func_base;

        benchmark::ConsoleReporter::ReportRuns(reports);
    }
};

int main(int argc, char* argv[])
{
    std::cout << "Dossier d'execution : " << std::filesystem::current_path() << std::endl;
    
    char arg_out[] = "--benchmark_out=results.json";
    char arg_format[] = "--benchmark_out_format=json";
    char arg_aggregates[] = "--benchmark_report_aggregates_only=true";
    char arg_filter[] = "--benchmark_filter=Batch"; // <-- Filtre uniquement les batchs
    
    std::vector<char*> args(argv, argv + argc);
    args.push_back(arg_out);
    args.push_back(arg_format);
    args.push_back(arg_aggregates);
    args.push_back(arg_filter);

    int new_argc = static_cast<int>(args.size());
    char** new_argv = args.data();

    ::benchmark::Initialize(&new_argc, new_argv);

    std::ofstream json_file("results.json");
    
    benchmark::JSONReporter json_reporter;
    json_reporter.SetOutputStream(&json_file);

    CustomConsoleReporter console_reporter;

    ::benchmark::RunSpecifiedBenchmarks(&console_reporter, &json_reporter);
    ::benchmark::Shutdown();

    return 0;
}