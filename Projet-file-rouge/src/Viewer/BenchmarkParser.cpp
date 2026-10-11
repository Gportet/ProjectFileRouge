#include "Viewer/BenchmarkParser.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

static std::string ExtractBaseName(const std::string& rawName) {
    size_t pos = rawName.find('/');
    std::string fullName = (pos != std::string::npos) ? rawName.substr(0, pos) : rawName;
    if (fullName.rfind("BM_", 0) == 0) {
        fullName = fullName.substr(3);
    }
    return fullName;
}

static std::string ExtractCategory(const std::string& functionName) {
    size_t pos = functionName.find('_');
    if (pos != std::string::npos) {
        return functionName.substr(0, pos);
    }
    return "General";
}

BenchmarkCategories LoadBenchmarkDataGroups(const std::string& filepath) {
    BenchmarkCategories categories;

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir " << filepath << std::endl;
        return categories;
    }

    json data;
    try {
        file >> data;
    } catch (const std::exception& e) {
        std::cerr << "Erreur JSON : " << e.what() << std::endl;
        return categories;
    }

    if (!data.contains("benchmarks") || !data["benchmarks"].is_array()) {
        return categories;
    }

    for (const auto& item : data["benchmarks"]) {
        std::string rawName = item.value("name", "");
        std::string fullName = ExtractBaseName(rawName);
        std::string category = ExtractCategory(fullName);
        std::string runType = item.value("run_type", "");
        std::string aggregateName = item.value("aggregate_name", "");

        auto& group = categories[category][fullName];
        group.functionName = fullName;
        group.category = category;

        double cyclesValue = 0.0;
        if (item.contains("Cycles/op") && item["Cycles/op"].is_number()) {
            cyclesValue = item["Cycles/op"].get<double>();
        } else if (item.contains("counters") && item["counters"].contains("Cycles/op")) {
            cyclesValue = item["counters"]["Cycles/op"].get<double>();
        } else {
            cyclesValue = item.value("cpu_time", 0.0);
        }

        if (runType == "iteration") {
            group.iterationCycles.push_back(cyclesValue);
            if (item.contains("cpu_time")) {
                group.iterationCpuTimes.push_back(item.value("cpu_time", 0.0));
            }
        } 
        else if (runType == "aggregate") {
            // Prise en charge de median, mean et p95
            if (aggregateName == "median" || aggregateName == "mean" || aggregateName == "p95") {
                group.p95Metrics.realTime = item.value("real_time", 0.0);
                group.p95Metrics.cpuTime = item.value("cpu_time", 0.0);
                group.p95Metrics.cyclesPerOp = cyclesValue;
                group.p95Metrics.itemsPerSecond = item.value("items_per_second", 0.0);
                group.p95Metrics.iterations = item.value("iterations", 0LL);
            } 
            else if (aggregateName == "stddev") {
                group.stdDevCpuTime = item.value("cpu_time", 0.0);
                group.stdDevCycles = cyclesValue;
            }
        }
    }

    return categories;
}