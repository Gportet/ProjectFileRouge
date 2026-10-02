#include "Viewer/BenchmarkParser.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::vector<BenchmarkResult> LoadBenchmarkResults(const std::string& filepath) {
    std::vector<BenchmarkResult> results;

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Impossible d'ouvrir le fichier : " << filepath << std::endl;
        return results;
    }

    json data = json::parse(file);

    // On parcourt le tableau "benchmarks" du JSON
    for (const auto& item : data["benchmarks"]) {
        BenchmarkResult res;
        res.name = item["name"].get<std::string>();

        // Extraction de la métrique personnalisée "Cycles/op" si elle existe
        if (item.contains("Cycles/op")) {
            res.cyclesPerOp = item["Cycles/op"].get<double>();
        } else {
            res.cyclesPerOp = 0.0;
        }

        results.push_back(res);
    }

    return results;
}