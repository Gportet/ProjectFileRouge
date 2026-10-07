#include "Viewer/BenchmarkParser.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

static std::string ExtractBaseName(const std::string& rawName) {
    size_t pos = rawName.find('/');
    if (pos != std::string::npos) {
        return rawName.substr(0, pos);
    }
    return rawName;
}

std::unordered_map<std::string, BenchmarkDataGroup> LoadBenchmarkDataGroups(const std::string& filepath) {
    std::unordered_map<std::string, BenchmarkDataGroup> groups;

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir " << filepath << std::endl;
        return groups;
    }

    json data = json::parse(file);

    for (const auto& item : data["benchmarks"]) {
        std::string rawName = item.value("name", "");
        std::string baseName = ExtractBaseName(rawName);
        std::string runType = item.value("run_type", "");
        std::string aggregateName = item.value("aggregate_name", "");

        auto& group = groups[baseName];
        group.functionName = baseName;

        if (runType == "iteration") {
            if (item.contains("cpu_time")) {
                group.iterationCycles.push_back(item["cpu_time"].get<double>());
            }
            if (item.contains("cpu_time")) {
                group.iterationCpuTimes.push_back(item["cpu_time"].get<double>());
            }
        } 
        else if (runType == "aggregate") {
            if (aggregateName == "p95") {
                group.p95Metrics.realTime = item.value("real_time", 0.0);
                group.p95Metrics.cpuTime = item.value("cpu_time", 0.0);
                // On recupere cpu_time faute de cle Cycles/op
                group.p95Metrics.cyclesPerOp = item.value("cpu_time", 0.0);
                group.p95Metrics.itemsPerSecond = item.value("items_per_second", 0.0);
                group.p95Metrics.iterations = item.value("iterations", 0LL);
            } 
            else if (aggregateName == "stddev") {
                group.stdDevCpuTime = item.value("cpu_time", 0.0);
                group.stdDevCycles = item.value("cpu_time", 0.0);
            }
        }
    }

    return groups;
}