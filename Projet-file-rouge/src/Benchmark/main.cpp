#include <benchmark/benchmark.h>
#include <vector>
#include <iostream>
#include <filesystem>
#if defined(_MSC_VER)
    #include <intrin.h>
#elif defined(__GNUC__) || defined(__clang__)
    #include <x86intrin.h>
#endif


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