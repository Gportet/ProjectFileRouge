#include <benchmark/benchmark.h>
#include <vector>
#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>

class CustomConsoleReporter : public benchmark::ConsoleReporter {
private:
    std::string current_category = "";
    std::string current_func_base = "";

    std::string ExtractCategory(const std::string& name) const {
        if (name.find("Batch") != std::string::npos) return "BATCH";
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
    std::cout << "Dossier d'execution : " << std::filesystem::current_path() << "\n\n";
    
    std::cout << "=== OPTION D'EXECUTION ===\n";
    std::cout << "1. Executer TOUS les benchmarks\n";
    std::cout << "2. Executer uniquement les BATCHS (par defaut)\n";
    std::cout << "Choix [1/2] (Entree = 2) : ";

    std::string choice;
    std::getline(std::cin, choice);

    char arg_out[] = "--benchmark_out=results.json";
    char arg_format[] = "--benchmark_out_format=json";
    char arg_filter[] = "--benchmark_filter=Batch";
    
    std::vector<char*> args(argv, argv + argc);
    args.push_back(arg_out);
    args.push_back(arg_format);

    if (choice != "1") {
        args.push_back(arg_filter);
        std::cout << "-> Lancement des traitements par batchs uniquement...\n\n";
    } else {
        std::cout << "-> Lancement de la totalite des benchmarks...\n\n";
    }

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