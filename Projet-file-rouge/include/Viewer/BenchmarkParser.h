#pragma once

#include <string>
#include <vector>


struct BenchmarkResult {
    std::string name;        // ex: "BM_Vec4Addition"
    double size = 0.0;       // ex: 250000
    double cyclesPerOp = 0.0;// ex: 1.99
    std::string runType;     // ex: "iteration" ou "aggregate"
    std::string aggregateName; // ex: "mean", "median", "p95"
    bool selected = true;    // Pour cocher/décocher dans ImGui
};

std::vector<BenchmarkResult> LoadBenchmarkResults(const std::string& filepath);