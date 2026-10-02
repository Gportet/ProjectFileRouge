#include <benchmark/benchmark.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <intrin.h>
#include <iostream>
#include <filesystem>
#include "MathLib/Vec4.h"

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